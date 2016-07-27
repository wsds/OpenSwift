/*
 * Excutor.cpp
 *
 *  Created on: 2016-5-12
 *      Author: wsds
 */

#include "Excutor.h"


/*
 * YES==true==x|(x is number && x!=0)==str|(str is string && str.length>0)==a|(a is JSON && a.length>0)
 * NO==false==x|(x is number && x==0)==str|(str is string && str.length==0)==a|(a is JSON && a.length==0)==null==NULL==undefined
 */
bool Excutor::checkCondition(MemorySpace* code_condition, Context* context) {
	MemorySpace * resolved_condition;
	if (code_condition->type == TYPE_JSON) {
		DMJSON * code_block = (DMJSON *) code_condition->pointer;
		if (code_block->type == 10) { //Expression
			resolved_condition = this->resolve_Expression(code_block, context);
		} else if (code_block->type == 32) { //FC
			resolved_condition = this->resolve_FunctionCall(code_block, context);
		} else if (code_block->type == 0) { //JSON
			if (code_block->dm_list->length > 0) {
				return true;
			} else {
				return false;
			}
		}
	} else if (code_condition->type == TYPE_CODE_NAME) {
		CodeName * code_name = (CodeName *) code_condition;
		if (code_name->nameType == 0xaa) { //in_expression
			resolved_condition = context->lookup(code_name);
		}
	} else {
		resolved_condition = code_condition;
	}

	if (resolved_condition == NULL) {
		return false;
	}
	if (resolved_condition->type == TYPE_BOOL) {
		DMBool * dm_bool = (DMBool *) resolved_condition;
		if (dm_bool == context->dm_true) {
			return true;
		}
	} else if (resolved_condition->type == TYPE_NUMBER) {
		DMInt32 * dm_int32 = (DMInt32 *) resolved_condition;
		if (dm_int32->number != 0) {
			return true;
		}
	} else if (resolved_condition->type == TYPE_STRING) {
		DMString * dm_string = (DMString *) resolved_condition;
		if (dm_string->used_length > 0) {
			return true;
		}
	}
	return false;
}

MemorySpace* Excutor::resolve_IfBlock(DMJSON* if_block, Context* context) {
	if (if_block->block_start != 1) {
		//error if block
		return NULL;
	}
	MemorySpace * code_condition = if_block->dm_list->get(0);

	bool check_result = this->checkCondition(code_condition, context);

	if (check_result == true) {
		this->run_CodeBlock(if_block, context, 1);
		context->skip_else = true;
	} else {
		context->skip_else = false;
	}

	return NULL;
}

MemorySpace* Excutor::resolve_ElseIfBlock(DMJSON* else_if_block, Context* context) {
	if (context->skip_else == true) {
		return NULL;
	}
	bool check_result = false;
	if(else_if_block->block_start == 0){
		check_result = true;
	}else if (else_if_block->block_start != 1) {
		//error if block
		return NULL;
	}else{
		MemorySpace * code_condition = else_if_block->dm_list->get(0);
		check_result = this->checkCondition(code_condition, context);
	}

	if (check_result == true) {
		this->run_CodeBlock(else_if_block, context, else_if_block->block_start);
		context->skip_else = true;
	} else {
		context->skip_else = false;
	}

	return NULL;
}

MemorySpace* Excutor::resolve_ForBlock(DMJSON* for_block, Context* context) {
	return NULL;
}

MemorySpace* Excutor::resolve_WhileBlock(DMJSON* while_block, Context* context) {

	if (while_block->block_start != 1) {
		//error if block
		return NULL;
	}
	MemorySpace * code_condition = while_block->dm_list->get(0);

	bool check_result = this->checkCondition(code_condition, context);
	while (check_result == true) {
		this->run_CodeBlock(while_block, context, 1);
		check_result = this->checkCondition(code_condition, context);

	}
	return NULL;
}

