/*
 * Context.cpp
 *
 *  Created on: 2016-5-12
 *      Author: wsds
 */

#include "Context.h"

void Context::initialize() {
    this->globle = newDMJSON();
    this->import = newDMJSON();

    this->dm_true = newDMBool();
    this->dm_true->value = 0xaa;
    this->dm_false = newDMBool();
    this->dm_null = swift_malloc(0);
    this->skiped = swift_malloc(0);

    this->skip_else = true;

    this->callstack = newDMCallstack();
    this->current_block = this->callstack->block_calls + this->callstack->callstack_top;
    this->current_block->local = newDMJSON();
    this->current_block->upper_syntax = this->current_block->local->self;

//    this->syntax_chain = newDMSyntaxChain();
//    this->syntax_chain->chain[0] = this->current_block->local;
//    this->syntax_chain->chain_top++;

    this->operator_map = newDMOperatorMap();
    this->function_call = newDMJSON();

    this->name = (MemorySpace *) newDMString(0);
    this->selector = newDMString(0);
}

Context *newDMContext() {
    MemorySpace *space = swift_malloc(sizeof(Context));
    space->type = TYPE_CONTEXT;

    Context *context = (Context *) space->pointer;
    context->self = space;
    context->initialize();
    return context;
}

void Context::resolveCodeName(CodeName *code_name) {

    int last_index = code_name->max_length;
    int name_length = code_name->max_length;
    char localChar;

    for (int i = 0; i < code_name->max_length; i++) {
        localChar = code_name->char_string[i];

        if (localChar == '.') {
            last_index = i + 1;
            name_length = i;
            break;
        } else if (localChar == '#') {
            last_index = i;
            name_length = i;
            break;
        }
    }

    this->name->pointer = code_name->char_string;
    this->name->length = name_length;

    this->selector->char_string = code_name->char_string + last_index;
    this->selector->max_length = code_name->max_length - last_index;
    this->selector->used_length = this->selector->max_length;
}

MemorySpace *Context::lookup(CodeName *code_name) {
    MemorySpace *object = NULL;

    this->resolveCodeName(code_name);

    if (*((DMString *) this->name) == "this") {
        if (this->current_block->parent != NULL) {
            object = this->current_block->parent->self;
        } else {
            //runtime error
        }

    } else {
        object = this->current_block->local->get(this->name);

        if (object == NULL && this->current_block->parent != NULL) {
            //object = this->current_block->parent->get(this->name);
        }

        if (object == NULL) {
            object = this->import->get(this->name);
        }

        if (object == NULL) {
            object = this->globle->get(this->name);
        }

        if (object == NULL) {
            MemorySpace *upper_syntax = this->current_block->upper_syntax;
            DMJSON *instance;
            DefinedFunction *dm_function;

            while (true) {
                if (upper_syntax == NULL) {
                    break;
                }
                if (upper_syntax->type == TYPE_FUNCTION) {
                    dm_function = (DefinedFunction *) upper_syntax->pointer;
                    object = dm_function->local->get(this->name);
                    upper_syntax = dm_function->upper_syntax;
                } else if (upper_syntax->type == TYPE_JSON) {
                    instance = (DMJSON *) upper_syntax->pointer;
                    if (instance->type != 1) {
                        //runtime error
                    }
                    object = instance->get(this->name);
                    upper_syntax = instance->upper_syntax;
                }
                if (object != NULL) {
                    break;
                }

            }
        }
    }


    if (this->selector->max_length == 0) {

    } else {
        if (object != NULL && object->type == TYPE_JSON) {
            DMJSON *instance = (DMJSON *) object->pointer;
            if (instance->type != 0 && instance->type != 1) {
                //runtime error
            }
            object = instance->get(this->selector);
        } else {
            //type error
        }
    }
    return object;

}

DMKeyValue *Context::lookupKV(CodeName *code_name) {
    DMKeyValue *dm_key_value = NULL;
    MemorySpace *object = NULL;

    this->resolveCodeName(code_name);

    if (*((DMString *) this->name) == "this") {
        if (this->current_block->parent != NULL) {
            object = this->current_block->parent->self;
        } else {
            //runtime error
        }

    } else {
        dm_key_value = this->current_block->local->getKV(this->name);

        if (dm_key_value == NULL) {
            dm_key_value = this->import->getKV(this->name);
        }

        if (dm_key_value == NULL) {
            dm_key_value = this->globle->getKV(this->name);
        }

        if (dm_key_value == NULL) {
            MemorySpace *upper_syntax = this->current_block->upper_syntax;
            DMJSON *instance;
            DefinedFunction *dm_function;

            while (true) {
                if (upper_syntax == NULL) {
                    break;
                }
                if (upper_syntax->type == TYPE_FUNCTION) {
                    dm_function = (DefinedFunction *) upper_syntax->pointer;
                    dm_key_value = dm_function->local->getKV(this->name);
                    upper_syntax = dm_function->upper_syntax;
                } else if (upper_syntax->type == TYPE_JSON) {
                    instance = (DMJSON *) upper_syntax->pointer;
                    if (instance->type != 1) {
                        //runtime error
                    }
                    dm_key_value = instance->getKV(this->name);
                    upper_syntax = instance->upper_syntax;
                }
                if (dm_key_value != NULL) {
                    break;
                }

            }
        }
    }


    if (this->selector->max_length == 0) {

    } else {
        if (dm_key_value != NULL && dm_key_value->value->type == TYPE_JSON) {
            DMJSON *instance = (DMJSON *) dm_key_value->value->pointer;
            if (instance->type != 0 && instance->type != 1) {
                //runtime error
            }
            dm_key_value = instance->getKV(this->selector);
        }
        if (object != NULL && object->type == TYPE_JSON) {
            DMJSON *instance = (DMJSON *) object->pointer;
            if (instance->type != 0 && instance->type != 1) {
                //runtime error
            }
            dm_key_value = instance->getKV(this->selector);
        } else {
            //type error
        }
    }
    return dm_key_value;
}


MemorySpace *Context::lookupContainer(CodeName *code_name) {
    MemorySpace *object = NULL;

    this->resolveCodeName(code_name);

    if (*((DMString *) this->name) == "this") {
        if (this->current_block->parent != NULL) {
            object = this->current_block->parent->self;
        } else {
            //runtime error
        }

    } else {

        object = this->current_block->local->get(this->name);

        if (object == NULL) {
            object = this->import->get(this->name);
        }

        if (object == NULL) {
            object = this->globle->get(this->name);
        }

        if (object == NULL) {
            MemorySpace *upper_syntax = this->current_block->upper_syntax;
            DMJSON *instance;
            DefinedFunction *dm_function;

            while (true) {
                if (upper_syntax == NULL) {
                    break;
                }
                if (upper_syntax->type == TYPE_FUNCTION) {
                    dm_function = (DefinedFunction *) upper_syntax->pointer;
                    object = dm_function->local->get(this->name);
                    upper_syntax = dm_function->upper_syntax;
                } else if (upper_syntax->type == TYPE_JSON) {
                    instance = (DMJSON *) upper_syntax->pointer;
                    if (instance->type != 1) {
                        //runtime error
                    }
                    object = instance->get(this->name);
                    upper_syntax = instance->upper_syntax;
                }
                if (object != NULL) {
                    break;
                }

            }
        }
    }

    return object;
}
