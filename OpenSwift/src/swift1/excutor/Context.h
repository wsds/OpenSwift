/*
 * Context.h
 *
 *  Created on: 2016-5-12
 *      Author: wsds
 */

#ifndef CONTEXT_H_
#define CONTEXT_H_
#include "DataSpace.h"
#include "OperatorMap.h"

class Context {
public:
	MemorySpace * self;

	void initialize();

	DMJSON * globle;
	DMJSON * import;

	DMBool * dm_true;
	DMBool * dm_false;
	MemorySpace * dm_null;
	MemorySpace * skiped;

	bool skip_else;

	Callstack * callstack;

	BlockCall * current_block;

//	SyntaxChain * syntax_chain;
	OperatorMap * operator_map;
	DMJSON *function_call;

	MemorySpace * name;
	DMString * selector;
	bool is_this;
	void resolveCodeName(CodeName * code_name);
	MemorySpace * lookup(CodeName * code_name);
	DMKeyValue * lookupKV(CodeName * code_name);
	MemorySpace * lookupContainer(CodeName * code_name);

};

Context * newDMContext();

#endif /* CONTEXT_H_ */
