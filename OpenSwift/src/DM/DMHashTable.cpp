#include "DMHashTable.h"

DMHashTable * newDMHashTable() {
	MemorySpace * space = swift_malloc(sizeof(DMHashTable));
	space->type = TYPE_HASH_TABLE;

	DMHashTable * dm_hashTable = (DMHashTable *) space->pointer;
	dm_hashTable->self = space;
	dm_hashTable->initialize();
	return dm_hashTable;
}

DMKeyValue * newDMKeyValue() {
	MemorySpace * space = swift_malloc(sizeof(DMKeyValue));
	space->type = TYPE_KEY_VALUE;

	DMKeyValue * dm_key_value = (DMKeyValue *) space->pointer;
	dm_key_value->self = space;
	return dm_key_value;
}

void freeDMKeyValue(DMKeyValue * dm_key_value) {
	MemorySpace * space = dm_key_value->self;
	swift_free(space);
}

TraverseCallback::TraverseCallback() {
	this->onGetData = NULL;
}

void DMHashTable::free() {
	swift_free(this->entry_map);
	swift_free(this->self);
}

void DMHashTable::traverse(TraverseCallback * traverseCallback) {

	if (traverseCallback->onGetData == NULL) {
		return;
	}

	MemorySpace ** elements = (MemorySpace **) this->entry_map->pointer;

	for (int index = 0; index < this->max_size; index++) {

		MemorySpace * element_space = *(elements + index);
		DMKeyValue * element = NULL;

		if (element_space != NULL) {
			do {
				element = (DMKeyValue *) element_space->pointer;
				traverseCallback->onGetData(element_space);
				element_space = element->next;
			} while (element_space != NULL);
		}
	}
}

MemorySpace* DMHashTable::get(MemorySpace* key) {

	unsigned int hash = 0;
	if (key->length > 0) {
		hash = dictGenHashFunction1(key->pointer, key->length);
	} else {
		hash = dictGenHashFunction1(&(key->pointer), 4);
	}
	int index = hash % this->max_size;

	MemorySpace ** elements = (MemorySpace **) this->entry_map->pointer;

	if (*(elements + index) != NULL) {
		MemorySpace * brother_space = *(elements + index);
		DMKeyValue * brother = NULL;
		do {
			brother = (DMKeyValue *) brother_space->pointer;
			if (compareMemeorySpace(brother->key, key) == true) {

				MemorySpace * value = brother->value;

				return value;
			}
			brother_space = brother->next;
		} while (brother_space != NULL);
	}

	return NULL;
}

DMKeyValue* DMHashTable::getKV(MemorySpace* key) {

	unsigned int hash = 0;
	if (key->length > 0) {
		hash = dictGenHashFunction1(key->pointer, key->length);
	} else {
		hash = dictGenHashFunction1(&(key->pointer), 4);
	}
	int index = hash % this->max_size;

	MemorySpace ** elements = (MemorySpace **) this->entry_map->pointer;

	if (*(elements + index) != NULL) {
		MemorySpace * brother_space = *(elements + index);
		DMKeyValue * brother = NULL;
		do {
			brother = (DMKeyValue *) brother_space->pointer;
			if (compareMemeorySpace(brother->key, key) == true) {

				return brother;
			}
			brother_space = brother->next;
		} while (brother_space != NULL);
	}

	return NULL;
}

DMKeyValue* DMHashTable::set(MemorySpace* key, MemorySpace* value) {

	unsigned int hash = 0;
	if (key->length > 0) {
		hash = dictGenHashFunction1(key->pointer, key->length);
	} else {
		hash = dictGenHashFunction1(&(key->pointer), 4);
	}
	int index = hash % this->max_size;

	MemorySpace ** elements = (MemorySpace **) this->entry_map->pointer;

	if (*(elements + index) != NULL) {
		MemorySpace ** brother_space_index = elements + index;
		MemorySpace * brother_space = *(brother_space_index);
		DMKeyValue * brother = NULL;

		do {
			brother = (DMKeyValue *) brother_space->pointer;
			if (compareMemeorySpace(brother->key, key) == true) {
				//The developer will decide whether the old value will be freed.
				brother->value = value; //replace value
				brother->is_replaced = 0xf1;

				//the old key will be freed by default.

				if (brother->key != key) {
					MemorySpace * old_key = brother->key;
					swift_de_reference(old_key);
				}
				brother->key = key;

				return brother;
			}
			brother_space = brother->next;

		} while (brother_space != NULL);
	}

	DMKeyValue* element = newDMKeyValue();
	element->key = key;
	element->value = value;
	element->hash = hash;

	if (*(elements + index) == NULL) {
		*(elements + index) = element->self;
	}

	else {
		int level = 1;
		MemorySpace * brother_space = *(elements + index);
		DMKeyValue * brother = (DMKeyValue *) brother_space->pointer;
		while (brother->next != NULL) {
			level++;
			brother_space = brother->next;
			brother = (DMKeyValue *) brother_space->pointer;
		}
		brother->next = element->self;
		element->level = level;
	}

	this->length++;
	if (this->length > this->threshold) {
		this->resize(); //asynchronous//todo
	}

	return element; //new entry
}

void DMHashTable::push(DMKeyValue* dm_key_value) {
	MemorySpace* key = dm_key_value->key;

	unsigned int hash = 0;
	if (key->length > 0) {
		hash = dictGenHashFunction1(key->pointer, key->length);
	} else {
		hash = dictGenHashFunction1(&(key->pointer), 4);
	}

	int index = hash % this->max_size;

	dm_key_value->hash = hash;

	MemorySpace ** elements = (MemorySpace **) this->entry_map->pointer;
	if (*(elements + index) == NULL) {
		*(elements + index) = dm_key_value->self;
	}

	else {
		int level = 1;
		MemorySpace * brother_space = *(elements + index);
		DMKeyValue * brother = (DMKeyValue *) brother_space->pointer;
		while (brother->next != NULL) {
			level++;
			brother_space = brother->next;
			brother = (DMKeyValue *) brother_space->pointer;
		}
		brother->next = dm_key_value->self;
		dm_key_value->level = level;
	}

	this->length++;
	if (this->length > this->threshold) {
		this->resize(); //asynchronous//todo
	}
}

DMKeyValue* DMHashTable::del(MemorySpace* key) {
	unsigned int hash = 0;
	if (key->length > 0) {
		hash = dictGenHashFunction1(key->pointer, key->length);
	} else {
		hash = dictGenHashFunction1(&(key->pointer), 4);
	}
	int index = hash % this->max_size;

	MemorySpace ** elements = (MemorySpace **) this->entry_map->pointer;
	MemorySpace ** old_space = NULL;

	if (*(elements + index) != NULL) {
		MemorySpace * brother_space = *(elements + index);
		old_space = elements + index;
		DMKeyValue * brother = NULL;

		do {
			brother = (DMKeyValue *) brother_space->pointer;
			if (compareMemeorySpace(brother->key, key) == true) {

				DMKeyValue * old_element = brother;
				*old_space = brother->next;
				this->length--;
//				swift_free(brother_space);
				return old_element;
			}
			brother_space = brother->next;
			old_space = &(brother->next);

		} while (brother_space != NULL);
	}

	return NULL;
}

bool DMHashTable::resize() {
	int old_index = 0;
	int index = 0;
	DMKeyValue* element = NULL;
	DMKeyValue* old_element = NULL;
	DMKeyValue* brother = NULL;
	int level = 1;
	int old_max_size = this->max_size;

	MemorySpace * old_entry_map = this->entry_map;
	MemorySpace ** old_elements = (MemorySpace **) this->entry_map->pointer;

	this->max_size = this->max_size * 2 + 7;
	this->threshold = (int) (this->max_size * 0.8);

	int mem_size = this->max_size * sizeof(MemorySpace *);

	this->entry_map = swift_malloc(mem_size);
	MemorySpace ** elements = (MemorySpace **) this->entry_map->pointer;

	for (old_index = 0; old_index < old_max_size; old_index++) {

		MemorySpace * element_space = *(old_elements + old_index);
		DMKeyValue * element = NULL;

		if (element_space != NULL) {
			do {
				element = (DMKeyValue *) element_space->pointer;
				index = element->hash % this->max_size;
				if (*(elements + index) == NULL) {
					*(elements + index) = element->self;
				}

				else {
					level = 1;
					MemorySpace * brother_space = *(elements + index);
					brother = (DMKeyValue *) brother_space->pointer;
					while (brother->next != NULL) {
						level++;
						brother_space = brother->next;
						brother = (DMKeyValue *) brother_space->pointer;
					}
					brother->next = element->self;
					element->level = level;
				}
				old_element = element;
				element_space = element->next;
				old_element->next = NULL;
			} while (element_space != NULL);
		}

	}

	swift_free(old_entry_map);

	return true;
}

bool DMHashTable::initialize() {
	if (is_initialized == 0x80011008) {
		return true;
	}
	this->max_size = 1 + 7;
	this->length = 0;
	this->threshold = (int) (this->max_size * 0.8);

	int mem_size = this->max_size * sizeof(MemorySpace *);

	this->entry_map = swift_malloc(mem_size);

	this->is_initialized = 0x80011008;

	return true;
}

static unsigned int dict_hash_function_seed1 = 5381;
/* MurmurHash2, by Austin Appleby
 * Note - This code makes a few assumptions about how your machine behaves -
 * 1. We can read a 4-byte value from any address without crashing
 * 2. sizeof(int) == 4
 *
 * And it has a few limitations -
 *
 * 1. It will not work incrementally.
 * 2. It will not produce the same results on little-endian and big-endian
 *    machines.
 */
unsigned int dictGenHashFunction1(void *key, int keyLength) {

	/* 'm' and 'r' are mixing constants generated offline.
	 They're not really 'magic', they just happen to work well.  */
	unsigned int seed = dict_hash_function_seed1;
	const unsigned int m = 0x5bd1e995;
	const int r = 24;

	/* Initialize the hash to a 'random' value */
	unsigned int h = seed ^ keyLength;

	/* Mix 4 bytes at a time into the hash */
	const unsigned char *data = (const unsigned char *) key;

	while (keyLength >= 4) {
		unsigned int k = *(unsigned int*) data;

		k *= m;
		k ^= k >> r;
		k *= m;

		h *= m;
		h ^= k;

		data += 4;
		keyLength -= 4;
	}

	/* Handle the last few bytes of the input array  */
	switch (keyLength) {
	case 3:
		h ^= data[2] << 16;
		break;
	case 2:
		h ^= data[1] << 8;
		break;
	case 1:
		h ^= data[0];
		h *= m;
		break;
	};

	/* Do a few final mixes of the hash to ensure the last few
	 * bytes are well-incorporated. */
	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return (unsigned int) h;
}

