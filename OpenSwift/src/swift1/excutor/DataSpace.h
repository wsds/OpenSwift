/*
 * DataSpace.h
 *
 *  Created on: 2016-5-12
 *      Author: wsds
 */

#ifndef DATASPACE_H_
#define DATASPACE_H_

#include "../SwiftData.h"

class DataSpace {
public:
    DataSpace();

    DMJSON *variables;
};

#define expression_element_size 32
#define operator_sequence_size 16

class ExpressionContainer {
public:
    MemorySpace *elements[expression_element_size];
    int numof_elements;

    int operator_sequence[operator_sequence_size];
    int operator_level[operator_sequence_size];
    int numof_operator;
    int numof_result;
    MemorySpace *result;
};

class BlockCall {
public:
    int level;
    DMJSON *code_block;
    DMJSON *local;
    DMJSON *parent;

    MemorySpace *upper_syntax;

    bool is_async;

    DMString *name;

    MemorySpace *result;
    int type;
};

#define callstack_size  20

class Callstack {
public:
    MemorySpace *self;
    int callstack_top;
    BlockCall block_calls[callstack_size];
    int expression_stack_top;
    ExpressionContainer expression_containers[callstack_size];
};

Callstack *newDMCallstack();

#define syntax_chain_size  10

class SyntaxChain {
public:
    MemorySpace *self;

    int chain_top;
    DMJSON *chain[syntax_chain_size];
};

SyntaxChain *newDMSyntaxChain();

class DefinedFunction {
public:
    MemorySpace *self;
    DMString *name;
    DMJSON *function_definition;
    MemorySpace *upper_syntax;
    DMJSON *local;
    DMJSON *parent;
};

#define native_function_param_size 8

class NativeFunction {
public:
    MemorySpace *self;
    char *definition;
    DMString *name;
    DMString *params[native_function_param_size];
    int numof_param;

    MemorySpace *upper_syntax;

    MemorySpace *(*doSomething)(DMJSON *local, NativeFunction *native_function);
};

class NativeOperatorFunction {
public:
    MemorySpace *self;

    MemorySpace *(*doSomething)(MemorySpace *left, MemorySpace *right);
};

NativeOperatorFunction *newDMNativeOperatorFunction();
NativeFunction *newDMNativeFunction();
DefinedFunction *newDMDefinedFunction();
NativeFunction *cloneNativeFunction(NativeFunction * native_function);


#endif /* DATASPACE_H_ */
