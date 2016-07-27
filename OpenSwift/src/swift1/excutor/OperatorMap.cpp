/*
 * OperatorMap.cpp
 *
 *  Created on: 2016-5-12
 *      Author: wsds
 */

#include "OperatorMap.h"

void OperatorMap::initialize() {
	this->operator_priority = newDMString(128);
	this->temp = newDMString(0);
	this->function_map = newDMJSON();

	this->setDeaultPriority();
	this->addDeaultNativeOperatorFunction();
}

void OperatorMap::setDeaultPriority() {
	this->addOperatorPriority("! ~ ++ --");
	this->addOperatorPriority("* / %");
	this->addOperatorPriority("+ -");
	this->addOperatorPriority("<< >>");
	this->addOperatorPriority("< > <= >=");
	this->addOperatorPriority("== !=");
	this->addOperatorPriority("& ^ |");
	this->addOperatorPriority("&& ||");
	this->addOperatorPriority("+= -= *= /=");
	this->addOperatorPriority("=");
}

void OperatorMap::addOperatorPriority(char* operatorStr) {
	*this->operator_priority * operatorStr * "\n";
}

OperatorMap* newDMOperatorMap() {
	MemorySpace * space = swift_malloc(sizeof(OperatorMap));
	space->type = TYPE_OPERATOR_MAP;

	OperatorMap * operator_map = (OperatorMap *) space->pointer;
	operator_map->self = space;
	operator_map->initialize();
	return operator_map;
}

int OperatorMap::getOperatorLevel(DMString* dm_operator) {
	int level = 0;
	char * char_string = this->operator_priority->char_string;
	int length = this->operator_priority->used_length;
	char operatorChar = -1;
	char localChar = 0;
	int last_index = 0;
	if (dm_operator->max_length == 1) {
		operatorChar = dm_operator->char_string[0];
	}
	for (int i = 0; i < length; i++) {
		localChar = char_string[i];
		if (localChar == '\n' || localChar == ' ') {
			if (operatorChar != -1) {
				if (i - last_index == 1) {
					if (operatorChar == char_string[i - 1]) {
						break;
					}
				}
			} else {
				this->temp->char_string = char_string + last_index;
				this->temp->max_length = i - last_index;
				this->temp->used_length = i - last_index;

				if (*dm_operator == *this->temp) {
					break;
				}
			}

			last_index = i + 1;
			if (localChar == '\n') {
				level++;
			}
		}

	}
	return level;

}

void OperatorMap::addOperatorFunction(char* map_key, MemorySpace* operator_function_space) {
	int length = getLength(map_key);
	DMString* key = newDMString(0);
	key->char_string = map_key;
	key->max_length = length;
	key->used_length = length;

	this->function_map->set((MemorySpace*) key, operator_function_space);

}

MemorySpace* number_add_number(MemorySpace* left, MemorySpace* right) {
	DMInt32* left_number = (DMInt32*) left;
	DMInt32* right_number = (DMInt32*) right;

	DMInt32* result_number = newDMInt32();
	result_number->number = left_number->number + right_number->number;
	return (MemorySpace*) result_number;
}

MemorySpace* number_multiply_number(MemorySpace* left, MemorySpace* right) {
	DMInt32* left_number = (DMInt32*) left;
	DMInt32* right_number = (DMInt32*) right;

	DMInt32* result_number = newDMInt32();
	result_number->number = left_number->number * right_number->number;
	return (MemorySpace*) result_number;
}

MemorySpace* string_add_number(MemorySpace* left, MemorySpace* right) {

	DMString* left_String = (DMString*) left;
	DMInt32* right_number = (DMInt32*) right;

	int targetLength = left_String->used_length + numberToString(right_number->number, NULL);

	DMString * result=newDMString(targetLength);

	result->copy(left_String->char_string, left_String->used_length);

	(*result) + right_number->number;
	return (MemorySpace *)result;
}

void OperatorMap::addDeaultNativeOperatorFunction() {
	NativeOperatorFunction* operator_function;

	operator_function = newDMNativeOperatorFunction();
	operator_function->doSomething = number_add_number;
	this->addOperatorFunction("number+number", operator_function->self);

	operator_function = newDMNativeOperatorFunction();
	operator_function->doSomething = number_multiply_number;
	this->addOperatorFunction("number*number", operator_function->self);

	operator_function = newDMNativeOperatorFunction();
	operator_function->doSomething = string_add_number;
	this->addOperatorFunction("string+number", operator_function->self);

}

