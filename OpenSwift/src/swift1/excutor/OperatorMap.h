/*
 * OperatorMap.h
 *
 *  Created on: 2016-5-12
 *      Author: wsds
 */

#ifndef OPERATORMAP_H_
#define OPERATORMAP_H_

#include "DataSpace.h"

class OperatorMap {
public:
	MemorySpace * self;
	void initialize();

	DMJSON * function_map;

	DMString * operator_priority;

	DMString * temp;

	void setDeaultPriority();
	void addOperatorPriority(char * operatorStr);

	int getOperatorLevel(DMString * dm_operator);

	void addOperatorFunction(char * map_key, MemorySpace * operator_function_space);
	void addDeaultNativeOperatorFunction();
};

OperatorMap * newDMOperatorMap();

#endif /* OPERATORMAP_H_ */
