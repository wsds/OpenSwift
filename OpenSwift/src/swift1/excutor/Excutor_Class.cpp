/*
 * Excutor.cpp
 *
 *  Created on: 2016-5-12
 *      Author: wsds
 */

#include "Excutor.h"

MemorySpace *Excutor::resolve_ClassDefinition(DMJSON *class_definition, Context *context) {
    DMJSON *local = context->current_block->local;

    DMJSON *instance = NULL;

    if (class_definition->parent_name != NULL) {
        MemorySpace *parent_space = context->lookup((CodeName *) class_definition->parent_name);
        if (parent_space == NULL) {
            //error function name cannot be resolved
        }

        if (parent_space->type == TYPE_JSON) {
            DMJSON *parent = (DMJSON *) parent_space->pointer;
            instance = this->clone_Class(parent, context);
        }

    } else {
        instance = newDMJSON();
    }

    instance->type = 1;
    instance->name = class_definition->name;
    instance->definition = class_definition;
    instance->upper_syntax = context->current_block->upper_syntax;

    local->set((MemorySpace *) instance->name, instance->self);

    context->callstack->callstack_top++;
    context->current_block = context->callstack->block_calls + context->callstack->callstack_top;
    context->current_block->upper_syntax = instance->self;
    context->current_block->local = instance;

//    context->syntax_chain->chain[context->syntax_chain->chain_top] = class_definition;
//    context->syntax_chain->chain_top++;

    MemorySpace *sub_code;
    DMJSON *sub_code_block;
    MemorySpace *resolved_element;
    for (int i = 0; i < class_definition->dm_list->length; i++) {
        sub_code = class_definition->dm_list->get(i);
        if (sub_code->type == TYPE_JSON) {
            sub_code_block = (DMJSON *) sub_code->pointer;
            if (sub_code_block->type == 20) { //CD
                this->resolve_ClassDefinition(sub_code_block, context);
            } else if (sub_code_block->type == 30) { //FD
                resolved_element = this->resolve_FunctionDefinition(sub_code_block, context);
                if (resolved_element->type == TYPE_FUNCTION) {
                    DefinedFunction *dm_function = (DefinedFunction *) resolved_element->pointer;
                    dm_function->parent = instance;
                }
            } else if (sub_code_block->type == 10) { //Expression
                continue;// resolve the expressions after the CD FD and variables are defined.
            } else {
                //error invalid code.
            }
            this->excute(sub_code_block, context);
        } else if (sub_code->type == TYPE_CODE_NAME) {
            CodeName *code_name = (CodeName *) sub_code;
            if (code_name->nameType == 0xff) { //var
                context->current_block->local->set(sub_code, context->dm_null);
            } else {
                //error invalid code.
            }
        } else if (sub_code->type == TYPE_STRING) {
            int i = 1;
            i++;
            //resolve as comments.
        } else {
            //error invalid code.
        }
    }

    for (int i = 0; i < class_definition->dm_list->length; i++) {
        sub_code = class_definition->dm_list->get(i);
        if (sub_code->type == TYPE_JSON) {
            sub_code_block = (DMJSON *) sub_code->pointer;
            if (sub_code_block->type == 10) { //Expression
                this->resolve_Expression(sub_code_block, context);
            }
        }
    }

    context->current_block->local = NULL;
    context->callstack->callstack_top--;
    context->current_block = context->callstack->block_calls + context->callstack->callstack_top;
    return instance->self;
}


DMJSON *Excutor::clone_Class(DMJSON *instance, Context *context) {
    DMJSON *clone = newDMJSON();
    if (instance->parent_name != NULL) {
        clone->parent_name = instance->parent_name;
    } else {
        clone->parent_name = instance->name;
    }

    MemorySpace *sub_space;
    DMKeyValue *dm_key_value;
    MemorySpace *sub_key;
    MemorySpace *sub_value;
    MemorySpace *sub_value_clone;
    DMJSON *sub_json;
    DMString *sub_string;
    DMInt32 *sub_int32;
    DefinedFunction *sub_function;
    NativeFunction *sub_native_function;
    for (int i = 0; i < instance->dm_list->length; i++) {
        sub_space = instance->dm_list->get(i);
        if (sub_space->type == TYPE_KEY_VALUE) {
            dm_key_value = (DMKeyValue *) sub_space->pointer;
            sub_key = dm_key_value->key;
            sub_value = dm_key_value->value;

            if (sub_value->type == TYPE_JSON) {
                sub_json = (DMJSON *) sub_value->pointer;
                DMJSON *sub_json_clone = sub_json;
                if (sub_json->type == 1) { //sub class
                    sub_json_clone = this->clone_Class(sub_json, context);
                } else if (sub_json->type == 0) { //sub JSON
                    sub_json_clone = this->clone_Class(sub_json, context);
                } else {
                    //runtime error invalid field.
                }

                sub_value_clone = sub_json_clone->self;
            } else if (sub_value->type == TYPE_NUMBER) {
                sub_int32 = (DMInt32 *) sub_value;
                DMInt32 *sub_int32_clone = newDMInt32();
                sub_int32_clone->number = sub_int32->number;
                sub_value_clone = (MemorySpace *) sub_int32_clone;
            } else if (sub_value->type == TYPE_STRING) {
                sub_string = (DMString *) sub_value;
                DMString *sub_string_clone = newDMString(sub_string->used_length);
                sub_string_clone->copy(sub_string->char_string, sub_string->used_length);
                sub_value_clone = (MemorySpace *) sub_string_clone;
            } else if (sub_value->type == TYPE_FUNCTION) {
                sub_function = (DefinedFunction *) sub_value->pointer;
                DefinedFunction *sub_function_clone = newDMDefinedFunction();
                sub_function_clone->name = sub_function->name;
                sub_function_clone->function_definition = sub_function->function_definition;
                sub_function_clone->upper_syntax = sub_function->upper_syntax; //专注于语义时
                sub_function_clone->parent = clone;//类关系属于运行时
                sub_value_clone = sub_function_clone->self;
            } else if (sub_value->type == TYPE_NATIVE_FUNCTION) {
                sub_native_function = (NativeFunction *) sub_value->pointer;
                NativeFunction *sub_native_function_clone = cloneNativeFunction(sub_native_function);
                sub_native_function_clone->upper_syntax = clone->self;
                sub_value_clone = sub_native_function_clone->self;
            } else {
                //runtime error invalid field.
            }

            clone->set(sub_key, sub_value_clone);
        } else {
            //ignore, but warning. cannot resolved fields in JSON
        }

    }

    return clone;
}


MemorySpace *Excutor::resolve_JSON(DMJSON *dm_json, Context *context) {

    DMJSON *clone = newDMJSON();
    clone->parent_name = dm_json->parent_name;

    MemorySpace *sub_space;
    DMKeyValue *dm_key_value;
    MemorySpace *sub_key;
    MemorySpace *sub_value;
    MemorySpace *sub_value_clone;
    DMJSON *sub_json;
    for (int i = 0; i < dm_json->dm_list->length; i++) {
        sub_space = dm_json->dm_list->get(i);
        if (sub_space->type == TYPE_KEY_VALUE) {
            dm_key_value = (DMKeyValue *) sub_space->pointer;
            sub_key = dm_key_value->key;
            sub_value = dm_key_value->value;

            if (sub_value->type == TYPE_JSON) {
                sub_json = (DMJSON *) sub_value->pointer;
                if (sub_json->type == 10) {//E
                    sub_value_clone = this->resolve_Expression(sub_json, context);
                } else if (sub_json->type == 32) {//FC
                    sub_value_clone = this->resolve_FunctionCall(sub_json, context);
                } else if (sub_json->type == 30) { //FD
                    sub_value_clone = this->resolve_FunctionDefinition(sub_json, context);
                    DefinedFunction *sub_function_clone = (DefinedFunction *)sub_value_clone->pointer;
                    sub_function_clone->parent = clone;
                } else if (sub_json->type == 0) { //sub JSON
                    sub_value_clone = this->resolve_JSON(sub_json, context);
                } else if (sub_json->type == 71) {//Selector

                } else {
                    //runtime error invalid field.
                }

            } else if (sub_value->type == TYPE_NUMBER) {
                sub_value_clone = sub_value;
            } else if (sub_value->type == TYPE_STRING) {
                sub_value_clone = sub_value;
            } else {
                //runtime error invalid field.
            }

            clone->set(sub_key, sub_value_clone);
        } else {
            sub_value = dm_key_value->value;

            if (sub_value->type == TYPE_JSON) {
                sub_json = (DMJSON *) sub_value->pointer;
                if (sub_json->type == 10) {//E
                    sub_value_clone = this->resolve_Expression(sub_json, context);
                } else if (sub_json->type == 32) {//FC
                    sub_value_clone = this->resolve_FunctionCall(sub_json, context);
                } else if (sub_json->type == 30) { //FD
                    sub_value_clone = this->resolve_FunctionDefinition(sub_json, context);
                } else if (sub_json->type == 0) { //sub JSON
                    sub_value_clone = this->resolve_JSON(sub_json, context);
                } else if (sub_json->type == 71) {//Selector

                } else {
                    //runtime error invalid field.
                }

            } else if (sub_value->type == TYPE_NUMBER) {
                sub_value_clone = sub_value;
            } else if (sub_value->type == TYPE_STRING) {
                sub_value_clone = sub_value;
            } else {
                //runtime error invalid field.
            }

            clone->set(i, sub_value_clone, NOT_REPLACE);
        }

    }

    return clone->self;
}
