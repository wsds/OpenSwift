#include "Storage.h"

LocalStorage * LocalStorage::instance = NULL;

LocalStorage::LocalStorage() {
	this->is_initialized = false;
	this->initialize();
}

LocalStorage * LocalStorage::getInstance() {
	if (instance == NULL) {
		instance = new LocalStorage();
	}
	return instance;
}

bool LocalStorage::initialize() {
	if (this->is_initialized == true) {
		return false;
	}
	this->is_initialized = true;

	void ** static_spaces;
	static_spaces = get_static_spaces();

	if (static_spaces[6] == NULL) {
		this->dm_hashTable = newDMHashTable();
		this->dm_hashTable_space = dm_hashTable->self;
		static_spaces[6] = dm_hashTable->self;

	} else {
		this->dm_hashTable_space = (MemorySpace *) static_spaces[6];
		this->dm_hashTable = (DMHashTable *) this->dm_hashTable_space->pointer;
	}

//	JSLog("LocalStorage::initialize");

	return true;
}

DMString * LocalStorage::loadString(DMString * key) {
	if (this->dm_hashTable_space->pointer != (void *) this->dm_hashTable) {
		this->dm_hashTable = (DMHashTable *) this->dm_hashTable_space->pointer;
	}
	DMString * value = (DMString *) this->dm_hashTable->get((MemorySpace *) key);
	return value;
}

void LocalStorage::savaString(DMString * key, DMString * value) {
	if (this->dm_hashTable_space->pointer != (void *) this->dm_hashTable) {
		this->dm_hashTable = (DMHashTable *) this->dm_hashTable_space->pointer;
	}
	DMKeyValue * dm_key_value = this->dm_hashTable->set((MemorySpace *) key, (MemorySpace *) value);
//	if (old_value != NULL) {
//		map_free(old_value);
//	}
}

char * LocalStorage::loadString(const char * key) {
	int key_length = getLength1((char*) key);

	DMString * key_dm_string = newDMString(key_length);
	backward_memcpy(key_dm_string->char_string, (void *) key, key_length);

	LocalStorage * localStorage = LocalStorage::getInstance();
	DMString * data_dm_string = localStorage->loadString(key_dm_string);

	freeDMString(key_dm_string);

	if (data_dm_string == NULL) {
		return NULL;
	}

	char * result_str = data_dm_string->char_string;

	return result_str;
}

void LocalStorage::savaString(const char * key, const char * value) {

	int key_length = getLength1((char*) key);

	int data_length = getLength1((char*) value);

	DMString * key_dm_string = newDMString(key_length);
	backward_memcpy(key_dm_string->char_string, (void *) key, key_length);
	DMString * value_dm_string = newDMString(data_length + 1); //the last '\0' is add to fit the JNI call of env->NewStringUTF(result_str);
	backward_memcpy(value_dm_string->char_string, (void *) value, data_length);

	LocalStorage * localStorage = LocalStorage::getInstance();
	localStorage->savaString(key_dm_string, value_dm_string);
}
