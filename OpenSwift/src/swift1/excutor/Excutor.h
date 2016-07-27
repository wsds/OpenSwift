/*
 * Excutor.h
 *
 *  Created on: 2016-5-12
 *      Author: wsds
 */

#ifndef EXCUTOR_H_
#define EXCUTOR_H_

#include "Context.h"

class Excutor {
public:
    Excutor();

    static Excutor *instance;

    static Excutor *getInstance();

    bool is_initialized;

    bool initialize();

    void excute(DMJSON *code_block, Context *context);

    MemorySpace *resolve_meta(MemorySpace *meta, Context *context);

    MemorySpace *resolve_CodeBlock(DMJSON *code_block, Context *context);

    MemorySpace *run_CodeBlock(DMJSON *code_block, Context *context, int block_start);

    MemorySpace *resolve_JSON(DMJSON *dm_json, Context *context);

    MemorySpace *resolve_FunctionCall(DMJSON *function_call, Context *context);

    MemorySpace *resolve_FunctionCall(DMJSON *function_call, DefinedFunction *dm_function, Context *context);

    MemorySpace *resolve_FunctionCall(DMJSON *function_call, NativeFunction *native_function, Context *context);

    MemorySpace *resolve_FunctionReturn(DMJSON *function_return, Context *context);

    MemorySpace *resolve_FunctionDefinition(DMJSON *function_definition, Context *context);

    MemorySpace *resolve_ClassDefinition(DMJSON *class_definition, Context *context);

    DMJSON *clone_Class(DMJSON *instance, Context *context);

    bool checkCondition(MemorySpace *code_condition, Context *context);

    MemorySpace *resolve_IfBlock(DMJSON *if_block, Context *context);

    MemorySpace *resolve_ElseIfBlock(DMJSON *else_if_block, Context *context);

    MemorySpace *resolve_ForBlock(DMJSON *for_block, Context *context);

    MemorySpace *resolve_WhileBlock(DMJSON *while_block, Context *context);

    MemorySpace *resolve_Expression(DMJSON *expression, Context *context);

    void sortOperators(Context *context, ExpressionContainer *expression_container);

    void mergeExpression(Context *context, ExpressionContainer *expression_container);

    void resolveOperator(int operator_index, Context *context, ExpressionContainer *expression_container);

    MemorySpace *resolve_Operator(MemorySpace *left, MemorySpace *right, DMString *dm_operator, Context *context);

    void resolve_assignment(MemorySpace *left, MemorySpace *right, Context *context);

    void resolve_assignToCodeName(MemorySpace *code_name_space, MemorySpace *value, Context *context);

    DMString *type_str;

    void getTypeOf(MemorySpace *element, DMString *type_str);
};

#endif /* EXCUTOR_H_ */
