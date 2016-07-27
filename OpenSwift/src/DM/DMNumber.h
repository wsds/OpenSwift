#ifndef DMNUMBER_H
#define DMNUMBER_H

#ifndef NULL
#define NULL 0
#endif /* NULL */

#include "MemoryMap.h"

class DMInt32 {
public:
	char type; //0xd0
	char isZero;
	char bit; //32
	char flag3;

	int number;

	int empty1;

	short reference;
	short index;
};

class DMInt64 {
public:
	char type; //0xd0
	char bit; //32
	char flag2;
	char flag3;

	long long number;

	short reference;
	short index;
};

class DMDouble {
public:
	char type; //0xd0
	char bit; //64
	char flag2;
	char flag3;

	double number;
	int empty1;
};

class DMBigNumber {
public:
	MemorySpace * self;

	MemorySpace * number_space;
	int bit; //>>64
	int range; //2^range?
	int precision; //0.5^precision?
};

DMInt32 * newDMInt32();
void freeDMInt32(DMInt32 * dm_int);

DMInt64 * newDMInt64();
void freeDMInt64(DMInt64 * dm_int);

DMDouble * newDMDouble();
void freeDMDouble(DMDouble * dm_int);

#endif /* DMNUMBER_H */

