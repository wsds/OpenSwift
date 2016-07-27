#ifndef DMSTRING_H
#define DMSTRING_H

#ifndef NULL
#define NULL 0
#endif /* NULL */

#include "MemoryMap.h"
#include "../baselib/temp_string.h"

class DMString {
public:
	char type; //0xf0
	char isZero; //true:IS_ZERO else false
	short used_length; //used_length<65536/2
	char * char_string;
	int max_length;
	short reference;
	short index;

	void resize(int new_length);

	DMString & operator+(int number);
	DMString & operator*(const char * char_string);
	DMString & operator*(DMString * dm_string);
	DMString & operator%(DMString * dm_string);
	DMString & operator%(int number);
	DMString & operator%(const char * char_string);
	bool operator==(const char * char_string);
	bool operator==(DMString & dm_string);

	void format(DMString * dm_string);
	void format(char * char_string, int targetLength);

	void copy(char * char_string, int targetLength);
	void copy(const char * char_string);
	void set(const char * char_string);
	void clear();

};

DMString * newDMString(int length);

void freeDMString(DMString * dm_string);

bool compareDMString(DMString * left, DMString * right);
#endif /* DMSTRING_H */

