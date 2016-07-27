#ifndef DMJSON_H
#define DMJSON_H

#include "MemoryMap.h"
#include "DMString.h"
#include "DMNumber.h"
#include "DMBool.h"
#include "DMHashTable.h"
#include "DMList.h"

class DMJSON {
public:
	MemorySpace * self;

	DMLIST * dm_list;
	MemorySpace * dm_list_space;//todo recover it whenever is used
	DMHashTable * dm_hashTable;
	MemorySpace * dm_hashTable_space;//todo recover it whenever is used

	int length;
	int is_initialized;

	int operation_flag;//todo for analyze Nested Loop Structure

//	DMHashTable *base_map;
//	DMString *json_string;

	MemorySpace *upper_syntax;

	DMJSON * definition;

	DMJSON * parent_json;
	MemorySpace * parent_json_space;
	DMString * parent_name;

	DMString * temp_key;

	int type; // 0:JSON 10:Expression 20:ClassDefinition 30:FunctionDefinition 41:IfBlock 42:ForBlock 43:ForInBlock 44:WhileBlock //? 32:FunctionCall 33:FunctionReturn 34:Block
	DMString * name;
	int block_start;

	bool initialize();
	void free();

	//API
	//list
	void set(int index, MemorySpace* value, bool isreplace);
	MemorySpace* get(int index, bool isdel);
	//TODO reorder

	//hash
	void set(MemorySpace* key, MemorySpace* value);
	MemorySpace* get(MemorySpace* key);
	DMKeyValue* getKV(MemorySpace* key);
	MemorySpace* del(MemorySpace* key);

	void push(MemorySpace * dm_key_value);

	void traverse(TraverseCallback * traverseCallback);

	//JSONBase
	bool put(DMString * key_chain, MemorySpace * value);
	bool set(DMString * key_chain, MemorySpace * value);
	MemorySpace * get(DMString * key_chain);
	DMKeyValue * getKV(DMString * key_chain);
	MemorySpace * getTargetJSON(DMString * key_chain);
	MemorySpace * del(DMString * key_chain);
};


class DefinedFunctionMirror {
public:
	MemorySpace *self;
	DMString *name;
	DMJSON *function_definition;
	MemorySpace *upper_syntax;
	DMJSON *local;
	DMJSON *parent;
};

DMJSON * newDMJSON();
bool exchangeSpace(MemorySpace * left, MemorySpace * right);

#endif /* DMJSON_H */

