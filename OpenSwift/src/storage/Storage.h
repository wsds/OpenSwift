#ifndef STORAGE_H
#define STORAGE_H

#include "../DM/DMHashTable.h"
#include "../DM/DMString.h"

class LocalStorage {
public:

	LocalStorage();

	bool is_initialized;

	static LocalStorage * instance;

	static LocalStorage * getInstance();

	bool initialize();
	void save();

	MemorySpace * dm_hashTable_space;
	DMHashTable * dm_hashTable;
	DMString * loadString(DMString * key);
	void savaString(DMString * key, DMString * value);

	char * loadString(const char * key);
	void savaString(const char * key, const char * value);
};

#endif /* STORAGE_H */
