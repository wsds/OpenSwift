#ifndef DMLIST_H
#define DMLIST_H

#include "MemoryMap.h"

#ifndef NULL
#define NULL 0
#endif /* NULL */

#define REPLACE true
#define NOT_REPLACE false
#define DEL true
#define NOT_DEL false

class DMLIST {
public:
	MemorySpace * self;//0xe1

	MemorySpace *  elements_space;

	int length;
	int tail;
	int head;

	int max_size;
	int threshold;

	int is_initialized;

	//API
	MemorySpace * get(int index);
	MemorySpace * get(int index, bool isdel);

	MemorySpace * del(int index);


	void set(int index, MemorySpace * value);
	void set(int index, MemorySpace * value, bool isreplace);

	void add(int index, MemorySpace * value);
	MemorySpace * replace(int index, MemorySpace * value);

	int findValue(MemorySpace * value);

	void traverse(TraverseCallback * traverseCallback);
	//resize O(n)
	bool resize();

	//initialize default size=8;
	bool initialize();

	bool free();

	void traverse();

};

DMLIST * newDMLIST();

#endif /* DMLIST_H */

