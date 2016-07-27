/*
 * Excutor.cpp
 *
 *  Created on: 2016-5-12
 *      Author: wsds
 */

#include "Excutor.h"

MemorySpace *Excutor::resolve_Expression(DMJSON *expression, Context *context) {
    MemorySpace *result = NULL;
    ExpressionContainer *expression_container =
            context->callstack->expression_containers + context->callstack->expression_stack_top;
    context->callstack->expression_stack_top++;

    expression_container->numof_elements = expression->dm_list->length;
    MemorySpace *element;
    MemorySpace *resolved_element;
    DMJSON *container;
    for (int i = 0; i < expression_container->numof_elements; i++) {
        element = expression->get(i, NOT_DEL);

        if (element->type == TYPE_JSON) { //Expression FunctionCall Selector JSON  FD
            container = (DMJSON *) element->pointer;
            if (container->type == 10) {//E
                resolved_element = this->resolve_Expression(container, context);
            } else if (container->type == 32) {//FC
                resolved_element = this->resolve_FunctionCall(container, context);
            } else if (container->type == 30) { //FD
                resolved_element = this->resolve_FunctionDefinition(container, context);
            } else if (container->type == 71) {//Selector

            } else if (container->type == 0) {//JSON
                resolved_element = this->resolve_JSON(container, context);
            } else if (container->type == 1 || container->type == 1) {//Class instance
                resolved_element = container->self;
            } else {
                //report runtime error
            }
            expression_container->elements[i] = resolved_element;
        } else if (element->type == TYPE_CODE_NAME) {
            CodeName *code_name = (CodeName *) element;
            if (code_name->nameType == (char) 0xff) { //var
                expression_container->elements[i] = element;
            } else if (code_name->nameType == (char) 0xaa ||
                    code_name->nameType == (char) 0xcc||
                       code_name->nameType == (char) 0x66) { //in_expression (bug)RV (bug)FCV
                resolved_element = context->lookup(code_name);
                if (resolved_element != NULL) {
                    expression_container->elements[i] = resolved_element;
                } else {
                    //error code name cannot be resolved
                }
            } else {
                //error code name type in expression
            }
        } else if (element->type == TYPE_CODE_OPERATOR) {
            DMString *dm_operator = (DMString *) element;
            if ((*dm_operator) == "=") {
                MemorySpace *last_element = expression->get(i - 1, NOT_DEL);
                if (last_element != NULL && (last_element->type == TYPE_CODE_NAME || last_element->type == TYPE_JSON)) {
                    expression_container->elements[i - 1] = last_element;
                }
            }
            expression_container->elements[i] = element;
        } else {
            expression_container->elements[i] = element;
        }
    }

    this->sortOperators(context, expression_container);
    this->mergeExpression(context, expression_container);

	if (expression_container->numof_result == 1) {
		expression_container->result = expression_container->elements[0];
	} else if (expression_container->numof_result > 1) {
		DMJSON *result = newDMJSON();
		result->type = 10;

		MemorySpace *element;
		for (int i = 0; i < expression_container->numof_elements; i++) {
			element = expression_container->elements[i];
			if (element == NULL || element == context->skiped) {
				continue;
			}
			result->set(-1, element, NOT_REPLACE);
			expression_container->numof_result--;
			if (expression_container->numof_result < -0) {
				break;
			}
		}
		expression_container->result = result->self;
	}

    result = expression_container->result;

    context->current_block->result = result;

    context->callstack->expression_stack_top--;
    return result;
}

void Excutor::sortOperators(Context *context, ExpressionContainer *expression_container) {
    OperatorMap *operator_map = context->operator_map;
    MemorySpace *element;
    expression_container->numof_operator = 0;
    expression_container->numof_result = 0;

    int level, index;
    for (int i = 0; i < expression_container->numof_elements; i++) {
        element = expression_container->elements[i];
        if (element == NULL) {
            continue;
        }
        if (element->type == TYPE_CODE_OPERATOR) {
            DMString *dm_operator = (DMString *) element;
            level = operator_map->getOperatorLevel(dm_operator);
            expression_container->operator_level[expression_container->numof_operator] = level;
            expression_container->operator_sequence[expression_container->numof_operator] = i;
            expression_container->numof_operator++;
        }else{
        	expression_container->numof_result++;
        }
    }

    int swap_level;
    int swap_sequence;
    for (int i = 0; i < expression_container->numof_operator; i++) {
        level = 36;

        for (int j = i; j < expression_container->numof_operator; j++) { // exchanging sorting
            if (expression_container->operator_level[j] < level) {
                level = expression_container->operator_level[j];
                index = j;
            }
        }
        if (level == 36) {
            break;
        }
        swap_level = expression_container->operator_level[index];
        swap_sequence = expression_container->operator_sequence[index];
        expression_container->operator_level[index] = expression_container->operator_level[i];
        expression_container->operator_sequence[index] = expression_container->operator_sequence[i];
        expression_container->operator_level[i] = swap_level;
        expression_container->operator_sequence[i] = swap_sequence;
    }
}

void Excutor::mergeExpression(Context *context, ExpressionContainer *expression_container) {

    int operator_index;
    for (int i = 0; i < expression_container->numof_operator; i++) {
        operator_index = expression_container->operator_sequence[i];
        this->resolveOperator(operator_index, context, expression_container);
    }
}

void Excutor::resolveOperator(int operator_index, Context *context, ExpressionContainer *expression_container) {
    int left_index, right_index;
    MemorySpace *left = NULL;
    DMString *dm_operator;
    MemorySpace *right = NULL;
    MemorySpace *result;
    MemorySpace *element;

    dm_operator = (DMString *) expression_container->elements[operator_index];

    left_index = operator_index - 1;
    while (left_index >= 0) {
        element = expression_container->elements[left_index];
        if (element != context->skiped) {
            break;
        }
        left_index--;
    }
    if (left_index >= 0) {
        left = expression_container->elements[left_index];
    }

    right_index = operator_index + 1;
    if (right_index < expression_container->numof_elements) {
        right = expression_container->elements[right_index];
    }

    if ((*dm_operator) == "=") {
        this->resolve_assignment(left, right, context);
    } else {
        result = this->resolve_Operator(left, right, dm_operator, context);

        expression_container->elements[left_index] = result;
        expression_container->elements[operator_index] = context->skiped;
        expression_container->elements[right_index] = context->skiped;
        expression_container->numof_result--;
    }

}

MemorySpace *Excutor::resolve_Operator(MemorySpace *left, MemorySpace *right, DMString *dm_operator, Context *context) {
    MemorySpace *result = NULL;
    OperatorMap *operator_map = context->operator_map;
    this->type_str->clear();

    this->getTypeOf(left, this->type_str);
    *this->type_str * dm_operator;
    this->getTypeOf(right, this->type_str);

    MemorySpace *operator_function_space = operator_map->function_map->get(this->type_str);
    if (operator_function_space == NULL) {
        //no rule to resolve the operator
        return NULL;
    }

    if (operator_function_space->type == TYPE_NATIVE_OPERATOR_FUNCTION) {
        NativeOperatorFunction *native_operator_function = (NativeOperatorFunction *) operator_function_space->pointer;
        result = native_operator_function->doSomething(left, right);
    } else if (operator_function_space->type == TYPE_FUNCTION) {
        DefinedFunction *dm_function = (DefinedFunction *) operator_function_space->pointer;
        context->function_call->block_start = 2;
        context->function_call->set(0, left, REPLACE);
        context->function_call->set(1, right, REPLACE);
        result = this->resolve_FunctionCall(context->function_call, dm_function, context);
    } else {
        //error
    }
    return result;
}

void Excutor::resolve_assignment(MemorySpace *left, MemorySpace *right, Context *context) {

    if (left->type == TYPE_CODE_NAME) {
        this->resolve_assignToCodeName(left, right, context);

    } else if (left->type == TYPE_JSON) {
        DMJSON *container = (DMJSON *) left->pointer;
        if (container->type == 10) {//Expression
            MemorySpace *code_name;
            MemorySpace *value;

            if (right->type != TYPE_JSON) {
                code_name = container->dm_list->get(0);
                this->resolve_assignToCodeName(code_name, right, context);
            } else {
                DMJSON *values = (DMJSON *) right->pointer;
                if (values->type != 10) {
                    //runtime error
                }
                for (int i = 0; i < container->dm_list->length && i < values->dm_list->length; i++) {
                    code_name = container->dm_list->get(i);
                    value = values->dm_list->get(i);
                    this->resolve_assignToCodeName(code_name, value, context);
                }
            }
        } else if (container->type == 71) {// Selector
        }
    }
}

void Excutor::resolve_assignToCodeName(MemorySpace *code_name_space, MemorySpace *value, Context *context) {
    if (value == NULL) {
        value = context->dm_null;
    }
    CodeName *code_name = (CodeName *) code_name_space;
    if (code_name->nameType == (char) 0xff) { //var
        context->current_block->local->set(code_name_space, value);
        swift_in_reference(value);
    } else  if ( code_name->nameType == (char) 0xbb) { //in_class
        context->current_block->local->set(code_name_space, value);
        swift_in_reference(value);
        if (value->type == TYPE_FUNCTION && context->current_block->local->type == 1) {
            DefinedFunction *dm_function = (DefinedFunction *) value->pointer;
            dm_function->parent = context->current_block->local;
        }
    } else {
        MemorySpace *old_value = context->lookup(code_name);
        if (old_value == NULL) {
            MemorySpace *container_space = context->lookupContainer(code_name);
            if (container_space != NULL || container_space->type == TYPE_JSON) {
                DMJSON *container = (DMJSON *) container_space->pointer;
                DMString *selector = context->selector;
                container->set(selector, value);
                if (value->type == TYPE_FUNCTION) {
                    DefinedFunction *dm_function = (DefinedFunction *) value->pointer;
                    dm_function->parent = container;
                }
            } else {
                //error code name cannot be resolved
            }
        } else if (old_value == value) {
            //do nothing
        } else if (old_value->type == value->type) {
            if (old_value->isZero == IS_ZERO) {
                if (value->type == TYPE_STRING) {
                    DMString *old_string = (DMString *) old_value;
                    DMString *new_string = (DMString *) value;
                    old_string->char_string = new_string->char_string;
                    old_string->used_length = new_string->used_length;
                    old_string->max_length = new_string->max_length;

                } else if (value->type == TYPE_NUMBER) {
                    DMInt32 *old_number = (DMInt32 *) old_value;
                    DMInt32 *new_number = (DMInt32 *) value;
                    old_number->number = new_number->number;
                } else {
                    //do something if goes here
                }
            } else {
                DMKeyValue *dm_key_value = context->lookupKV(code_name);
                if (dm_key_value == NULL) {
                    //runtime error
                } else if (dm_key_value->value != value) {
                    swift_de_reference(dm_key_value->value);
                    dm_key_value->value = value;
                    swift_in_reference(value);
                }
            }
        }

    }
}
