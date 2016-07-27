/*
 * DataSpace.cpp
 *
 *  Created on: 2016-5-12
 *      Author: wsds
 */

#include "DataSpace.h"

DataSpace::DataSpace() {

}

Callstack* newDMCallstack() {
	MemorySpace * space = swift_malloc(sizeof(Callstack));
	space->type = TYPE_CALLSTACK;

	Callstack * callstack = (Callstack *) space->pointer;
	callstack->self = space;

	for (int i = 0; i < callstack_size; i++) {
		BlockCall * block_call = callstack->block_calls + i;
		block_call->level = i;
	}

	return callstack;
}

SyntaxChain* newDMSyntaxChain() {
	MemorySpace * space = swift_malloc(sizeof(SyntaxChain));
	space->type = TYPE_SYNTAX_CHAIN;

	SyntaxChain * syntax_chain = (SyntaxChain *) space->pointer;
	syntax_chain->self = space;
	return syntax_chain;
}



NativeOperatorFunction* newDMNativeOperatorFunction() {
	MemorySpace * space = swift_malloc(sizeof(NativeOperatorFunction));
	space->type = TYPE_NATIVE_OPERATOR_FUNCTION;

	NativeOperatorFunction * native_operator_function = (NativeOperatorFunction *) space->pointer;
	native_operator_function->self = space;
	return native_operator_function;
}


NativeFunction *newDMNativeFunction(){
	MemorySpace * space = swift_malloc(sizeof(NativeFunction));
	space->type = TYPE_NATIVE_FUNCTION;

	NativeFunction * native_function = (NativeFunction *) space->pointer;
	native_function->self = space;
	return native_function;
}

DefinedFunction *newDMDefinedFunction(){
	MemorySpace * space = swift_malloc(sizeof(DefinedFunction));
	space->type = TYPE_FUNCTION;

	DefinedFunction * dm_function = (DefinedFunction *) space->pointer;
	dm_function->self = space;
	return dm_function;
}

NativeFunction *cloneNativeFunction(NativeFunction * native_function){
	MemorySpace * space = swift_malloc(sizeof(NativeFunction));
	backward_memcpy(space->pointer, native_function->self->pointer, sizeof(NativeFunction));

	NativeFunction * native_function_clone = (NativeFunction *) space->pointer;
	native_function->self = space;
	return native_function_clone;

}


