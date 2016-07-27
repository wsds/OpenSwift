#ifndef DMHASHTABLE_H
#define DMHASHTABLE_H

#include "MemoryMap.h"

#ifndef NULL
#define NULL 0
#endif /* NULL */

class DMKeyValue {
public:
	MemorySpace * self;

	MemorySpace * key;
	MemorySpace* value;

	unsigned int hash;


	MemorySpace* next;


	char level;
	char is_replaced;//true:0xf1; false: 0x00;
	char empty1;
	char empty2;

};


class DMHashTable {
public:
	MemorySpace * self;

	MemorySpace * entry_map;

	int max_size;
	int length;
	int threshold;

	int is_initialized;

	//get O(1)
	MemorySpace* get(MemorySpace * key);
	DMKeyValue* getKV(MemorySpace * key);


	//set O(1)
	DMKeyValue* set(MemorySpace * key, MemorySpace * value);
	void push(DMKeyValue* dm_key_value);

	//del O(1)
	DMKeyValue* del(MemorySpace * key);

	void traverse(TraverseCallback * traverseCallback);

	//resize O(n)
	bool resize();

	//initialize default size=8;
	bool initialize();

	void free();

};
DMHashTable * newDMHashTable();

DMKeyValue * newDMKeyValue();
void freeDMKeyValue(DMKeyValue * dm_key_value);

unsigned int dictGenHashFunction1(void *key, int keyLength);

#endif /* DMHASHTABLE_H */

