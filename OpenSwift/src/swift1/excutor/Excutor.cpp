/*
 * Excutor.cpp
 *
 *  Created on: 2016-5-12
 *      Author: wsds
 */

#include "Excutor.h"

Excutor::Excutor() {
    this->is_initialized = false;
    this->initialize();
}

Excutor *Excutor::instance = NULL;

Excutor *Excutor::getInstance() {
    if (instance == NULL) {
        instance = new Excutor();
    }
    return instance;
}

bool Excutor::initialize() {
    if (this->is_initialized == true) {
        return true;
    }

    this->type_str = newDMString(32);
    this->is_initialized = true;

    return true;
}

void Excutor::excute(DMJSON *code_block, Context *context) {

    DMString *code_string = newDMString(16);
    if (code_block->type == 10) { //Expression
        this->resolve_Expression(code_block, context);
    } else if (code_block->type == 20) { //CD
        this->resolve_ClassDefinition(code_block, context);
    } else if (code_block->type == 30) { //FD
        this->resolve_FunctionDefinition(code_block, context);
    } else if (code_block->type == 32) { //FC
        this->resolve_FunctionCall(code_block, context);
    } else if (code_block->type == 33) { //Return
        this->resolve_FunctionReturn(code_block, context);
    } else if (code_block->type == 51) { //IMPORT
    } else if (code_block->type == 61) { //ENUM
    } else if (code_block->type == 71) { //SELECTOR
    } else if (code_block->type == 0) { //JSON
    } else if (code_block->type == 34) { //ROOT
        this->resolve_CodeBlock(code_block, context);
    } else if (code_block->type == 41) { //IF
        this->resolve_IfBlock(code_block, context);
    } else if (code_block->type == 45) { //ELSEIF
        this->resolve_ElseIfBlock(code_block, context);
    } else if (code_block->type == 42) { //FOR
        this->resolve_ForBlock(code_block, context);
    } else if (code_block->type == 44) { //WHILE
        this->resolve_WhileBlock(code_block, context);
    } else if (code_block->type / 10 == 4) { //BLOCK
        this->resolve_CodeBlock(code_block, context);
    } else {
        //report runtime error
    }

}


MemorySpace *Excutor::resolve_meta(MemorySpace *meta, Context *context) {
    context->current_block->result = NULL;
    if (meta->type == TYPE_JSON) {
        DMJSON *sub_code_block;
        sub_code_block = (DMJSON *) meta->pointer;
        if (sub_code_block->type == 10) { //Expression
            this->resolve_Expression(sub_code_block, context);
        } else if (sub_code_block->type == 32) { //FC
            this->resolve_FunctionCall(sub_code_block, context);
        } else if (sub_code_block->type == 30) { //FD
            this->resolve_FunctionDefinition(sub_code_block, context);
        } else if (sub_code_block->type == 0) { //JSON
            this->resolve_JSON(sub_code_block, context);
        } else if (sub_code_block->type == 71) {

        } else {
            //error the meta cannot be resolved
        }
    } else if (meta->type == TYPE_CODE_NAME) {

        CodeName *code_name = (CodeName *) meta;
        if (code_name->nameType == (char) 0xcc || code_name->nameType == (char) 0x66) { //FCV RV
            MemorySpace *result = context->lookup(code_name);
            if (result != NULL) {
                context->current_block->result = result;
            } else {
                //error XXX is not found
            }

        } else {
            //console.log it when in auto log mode.
        }
    } else if (meta->type == TYPE_STRING) {
        context->current_block->result = meta;
    } else if (meta->type == TYPE_NUMBER) {
        context->current_block->result = meta;
    } else {
        //error invalid code.
    }

    return NULL;
}


MemorySpace *Excutor::resolve_CodeBlock(DMJSON *code_block, Context *context) {
    if (code_block->type != 34) {
        context->callstack->callstack_top++;
        context->current_block = context->callstack->block_calls + context->callstack->callstack_top;
    }
    if (context->current_block->local == NULL) {
        context->current_block->local = newDMJSON();
    }

    this->run_CodeBlock(code_block, context, 0);
    return NULL;
}

MemorySpace *Excutor::run_CodeBlock(DMJSON *code_block, Context *context, int block_start) {

    MemorySpace *sub_code;
    DMJSON *sub_code_block;

    for (int i = block_start; i < code_block->dm_list->length; i++) {
        sub_code = code_block->dm_list->get(i);
        if (sub_code->type == TYPE_JSON) {
            sub_code_block = (DMJSON *) sub_code->pointer;
            if (sub_code_block->type == 20) { //CD
                this->resolve_ClassDefinition(sub_code_block, context);
            } else if (sub_code_block->type == 30) { //FD
                this->resolve_FunctionDefinition(sub_code_block, context);
            }
        }
    }

    for (int i = block_start; i < code_block->dm_list->length; i++) {
        sub_code = code_block->dm_list->get(i);
        if (sub_code->type == TYPE_JSON) {
            sub_code_block = (DMJSON *) sub_code->pointer;
            if (sub_code_block->type == 20) { //CD
                continue;
            } else if (sub_code_block->type == 30) { //FD
                continue;
            } else if (sub_code_block->type == 0) { //JSON
                //console.log it when in auto log mode.
                continue;
            }
            this->excute(sub_code_block, context);
        } else if (sub_code->type == TYPE_CODE_NAME) {
            CodeName *code_name = (CodeName *) sub_code;
            if (code_name->nameType == 0xff) { //var
                context->current_block->local->set(sub_code, context->dm_null);
            } else {
                //console.log it when in auto log mode.
            }
        } else if (sub_code->type == TYPE_STRING) {
            //console.log it when in auto log mode.
        } else if (sub_code->type == TYPE_NUMBER) {
            //console.log it when in auto log mode.
        } else {
            //error invalid code.
        }

    }

    return NULL;
}


void Excutor::getTypeOf(MemorySpace *element, DMString *type_str) {
    if (element == NULL) {
        (*type_str) * "NULL";
        return;
    }
    if (element->type == TYPE_NUMBER) {
        (*type_str) * "number";
    } else if (element->type == TYPE_STRING) {
        (*type_str) * "string";
    } else if (element->type == TYPE_JSON) {
        DMJSON *json = (DMJSON *) element->pointer;
        if (json->type != 0) {
            (*type_str) * "##CODE##";
        } else if (json->parent_name != NULL) {
            (*type_str) * json->parent_name;
        } else if (json->name != NULL) {
            (*type_str) * json->name;
        } else {
            (*type_str) * "json";
        }
    } else {
        (*type_str) * "##CORE##";
    }
}

