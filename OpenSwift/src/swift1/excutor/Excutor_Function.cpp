/*
 * Excutor.cpp
 *
 *  Created on: 2016-5-12
 *      Author: wsds
 */

#include "Excutor.h"

MemorySpace *Excutor::resolve_FunctionCall(DMJSON *function_call, Context *context) {

    CodeName *function_name = (CodeName *) function_call->name;

    bool is_new = false;
    context->current_block->is_async = false;
    if (function_name->nameType == 0x82) {
        is_new = true;
    } else if (function_name->nameType == 0x83) {
        context->current_block->is_async = true;
    }

    MemorySpace *function_space = context->lookup(function_name);
    MemorySpace *result = NULL;
    if (function_space == NULL) {
        //error function name cannot be resolved
    }

    if (function_space->type == TYPE_FUNCTION) {

        DefinedFunction *dm_function = (DefinedFunction *) function_space->pointer;
        result = this->resolve_FunctionCall(function_call, dm_function, context);

    } else if (function_space->type == TYPE_NATIVE_FUNCTION) {

        NativeFunction *native_function = (NativeFunction *) function_space->pointer;
        result = this->resolve_FunctionCall(function_call, native_function, context);

    } else if (function_space->type == TYPE_JSON) {
        DMJSON *instance = (DMJSON *) function_space->pointer;
        if (instance->type == 1) {
//			if (is_new) {//todo here is a parse bug
            DMJSON *clone = this->clone_Class(instance, context);
            result = clone->self;
//			}

        } else {
            //error function
        }
    } else {
        //error function
    }
    context->current_block->result = result;
    return result;
}

MemorySpace *Excutor::resolve_FunctionCall(DMJSON *function_call, DefinedFunction *dm_function, Context *context) {

    BlockCall *next_block = context->callstack->block_calls + context->callstack->callstack_top + 1;
    next_block->upper_syntax = dm_function->upper_syntax;
    next_block->parent = dm_function->parent;

    if (next_block->local == NULL) {
        next_block->local = newDMJSON();
    } else {
        //todo clear the used local space
    }

    DMJSON *local = next_block->local;
    dm_function->local = local;

    DMJSON *function_definition = dm_function->function_definition;

    MemorySpace *param_name;
    MemorySpace *param;
    for (int i = 0; i < function_call->dm_list->length; i++) {
        param_name = function_definition->dm_list->get(i);
        param = function_call->dm_list->get(i);
        this->resolve_meta(param, context);

        if (context->current_block->result != NULL) {
            local->set(param_name, context->current_block->result);
        } else {
            //error XXX is not found
        }
    }

    context->callstack->callstack_top++;
    context->current_block = next_block;

    this->run_CodeBlock(function_definition, context, function_definition->block_start);

    MemorySpace *result = context->current_block->result;

    context->callstack->callstack_top--;
    context->current_block = context->callstack->block_calls + context->callstack->callstack_top;
    return result;
}

MemorySpace *Excutor::resolve_FunctionCall(DMJSON *function_call, NativeFunction *native_function, Context *context) {

    BlockCall *next_block = context->callstack->block_calls + context->callstack->callstack_top + 1;
    next_block->upper_syntax = native_function->upper_syntax;

    if (next_block->local == NULL) {
        next_block->local = newDMJSON();
    } else {
        //todo clear the used local space
    }

    DMJSON *local = next_block->local;

    MemorySpace *param_name;
    MemorySpace *param;
    for (int i = 0; i < function_call->dm_list->length && i < native_function->numof_param; i++) {
        param_name = (MemorySpace *) native_function->params[i];
        param = function_call->dm_list->get(i);
        this->resolve_meta(param, context);

        if (context->current_block->result != NULL) {
            local->set(param_name, context->current_block->result);
        } else {
        	local->set(param_name, NULL);
            //error XXX is not found
        }
    }

    context->callstack->callstack_top++;
    context->current_block = next_block;

    MemorySpace *result = native_function->doSomething(local, native_function);

    context->current_block->result = result;

    context->callstack->callstack_top--;
    context->current_block = context->callstack->block_calls + context->callstack->callstack_top;
    return result;

}

MemorySpace *Excutor::resolve_FunctionReturn(DMJSON *function_return, Context *context) {

    if (function_return->dm_list->length == 1) {
        MemorySpace *param = function_return->dm_list->get(0);
        this->resolve_meta(param, context);
        return NULL;
    }

    DMJSON *result = newDMJSON();
    result->type = 10;

    MemorySpace *param;
    for (int i = 0; i < function_return->dm_list->length; i++) {
        param = function_return->dm_list->get(i);
        this->resolve_meta(param, context);

        if (context->current_block->result != NULL) {
            result->set(-1, context->current_block->result, NOT_REPLACE);
        } else {
            result->set(-1, context->dm_null, NOT_REPLACE);
        }
    }

    context->current_block->result = result->self;

    return NULL;
}

MemorySpace *Excutor::resolve_FunctionDefinition(DMJSON *function_definition, Context *context) {

    DMJSON *local = context->current_block->local;

    DefinedFunction *dm_function = newDMDefinedFunction();

    dm_function->name = function_definition->name;
    dm_function->function_definition = function_definition;
    dm_function->upper_syntax = context->current_block->upper_syntax;
    if(context->current_block->local->type == 1){
    	dm_function->parent = context->current_block->local;
    }

    local->set((MemorySpace *) dm_function->name, dm_function->self);

    context->current_block->result = dm_function->self;

    return dm_function->self;
}

