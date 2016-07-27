#include "DMList.h"

DMLIST * newDMLIST() {
	MemorySpace * space = swift_malloc(sizeof(DMLIST));
	space->type = TYPE_LIST;
	DMLIST * dm_list = (DMLIST *) space->pointer;
	dm_list->self = space;
	dm_list->initialize();
	return dm_list;
}

void DMLIST::add(int index, MemorySpace * value) {

	if (index < 0) {
		index = this->length + 1 + index;
	}

	if (index < 0 || index > this->length) {
		return;
	}

	if (this->max_size <= this->length) {
		this->resize(); //synchronous
	}

	if (this->length + 1 > this->threshold) {
		this->resize(); //asynchronous
	}

	MemorySpace ** elements = (MemorySpace **) this->elements_space->pointer;

	int element_index = 0;

	if (index == this->length) { //right push

		element_index = this->tail % this->max_size;
		element_index = (element_index + this->max_size) % this->max_size;

		elements[element_index] = value;
		this->tail++;
		this->length++;
	} else if (index == 0) { //left push
		element_index = (this->head - 1) % this->max_size;
		element_index = (element_index + this->max_size) % this->max_size;

		elements[element_index] = value;
		this->head--;
		this->length++;

	} else if (index >= (this->length / 2)) { //right insert
		for (int i = 0; i < this->length - index; i++) {
			element_index = (this->tail - 1 - i) % this->max_size;
			element_index = (element_index + this->max_size) % this->max_size;

			elements[(element_index + 1) % this->max_size] = elements[element_index];
		}
		elements[element_index] = value;
		this->tail++;
		this->length++;

	} else if (index < (this->length / 2)) { //left insert
		for (int i = 0; i < index; i++) {
			element_index = (this->head + i) % this->max_size;
			element_index = (element_index + this->max_size) % this->max_size;

			elements[(element_index - 1 + this->max_size) % this->max_size] = elements[element_index];
		}
		elements[element_index] = value;
		this->head--;
		this->length++;
	}
}

MemorySpace * DMLIST::replace(int index, MemorySpace * value) {
	if (index < 0) {
		index = this->length + index;
	}

	if (index < 0 || index > this->length) {
		return NULL;
	}
	MemorySpace ** elements = (MemorySpace **) this->elements_space->pointer;

	int element_index = (this->head + index) % this->max_size;
	element_index = (element_index + this->max_size) % this->max_size;

	MemorySpace * old_value = elements[element_index];
	elements[element_index] = value;

	return old_value;
}

MemorySpace * DMLIST::get(int index) {
	if (index < 0) {
		index = this->length + index;
	}

	if (index < 0 || index > this->length) {
		return NULL;
	}
	MemorySpace ** elements = (MemorySpace **) this->elements_space->pointer;

	int element_index = (this->head + index) % this->max_size;
	element_index = (element_index + this->max_size) % this->max_size;

	MemorySpace * value = elements[element_index];

	return value;
}
MemorySpace * DMLIST::get(int index, bool isdel) {
	MemorySpace * value = this->get(index);
	if (isdel == DEL) {
		this->del(index);
	}
	return value;
}

int DMLIST::findValue(MemorySpace * value) {
	int index = -1;
	MemorySpace ** elements = (MemorySpace **) this->elements_space->pointer;
	int element_index = 0;

	for (int i = 0; i < this->length; i++) {
		element_index = (this->head + i) % this->max_size;
		element_index = (element_index + this->max_size) % this->max_size;

		if (elements[element_index] == value) {
			index = i;
			break;
		}
	}
	return index;
}

MemorySpace * DMLIST::del(int index) {
	if (index < 0) {
		index = this->length + index;
	}

	if (index < 0 || index > this->length) {
		return NULL;
	}
	MemorySpace ** elements = (MemorySpace **) this->elements_space->pointer;

	int element_index = (this->head + index) % this->max_size;
	element_index = (element_index + this->max_size) % this->max_size;

	MemorySpace * old_value = elements[element_index];
	elements[element_index] = NULL;

	if (index == this->length - 1) { //right pop
		this->tail--;
		this->length--;
	} else if (index == 0) { //left pop
		this->head++;
		this->length--;
	} else if (index >= (this->length / 2)) { //right del
		for (int i = this->length - index; i > 0; i--) {
			element_index = (this->tail - i) % this->max_size;
			element_index = (element_index + this->max_size) % this->max_size;

			elements[element_index] = elements[(element_index + 1) % this->max_size];
		}
		this->tail--;
		this->length--;

	} else if (index < (this->length / 2)) { //left del
		for (int i = index; i >= 0; i--) {
			element_index = (this->head + i) % this->max_size;
			element_index = (element_index + this->max_size) % this->max_size;

			elements[element_index] = elements[(element_index - 1 + this->max_size) % this->max_size];
		}
		this->head++;
		this->length--;
	}

	return old_value;
}

void DMLIST::set(int index, MemorySpace * value) {
	this->add(index, value);
}

void DMLIST::set(int index, MemorySpace * value, bool isreplace) {
	if (isreplace == REPLACE) {
		this->replace(index, value);
	} else {
		this->add(index, value);
	}
}

void DMLIST::traverse(TraverseCallback * traverseCallback) {
	if (traverseCallback->onGetData == NULL) {
		return;
	}

	MemorySpace ** elements = (MemorySpace **) this->elements_space->pointer;

	MemorySpace * element;
	int i = 0;
	for (i = this->head; i < this->tail; i++) {
		element = elements[i % this->max_size];
		traverseCallback->onGetData(element);
	}
}

bool DMLIST::resize() {
	MemorySpace * old_elements_space = this->elements_space;

	int new_max_size = this->max_size * 2;
	this->threshold = (int) (new_max_size * 0.8);
	int mem_size = new_max_size * sizeof(MemorySpace *);
	MemorySpace * new_elements_space = swift_malloc(mem_size);

	MemorySpace ** old_elements = (MemorySpace **) old_elements_space->pointer;
	MemorySpace ** new_elements = (MemorySpace **) new_elements_space->pointer;
	int i = 0;
	for (i = this->head; i < this->tail; i++) {
		new_elements[i % new_max_size] = old_elements[i % this->max_size];
	}

	this->elements_space = new_elements_space;
	this->max_size = new_max_size;

	swift_free(old_elements_space);
	return true;
}

bool DMLIST::initialize() {
	if (this->is_initialized == 0x80012008) {
		return false;
	}
	this->max_size = 4;
	this->length = 0;
	this->tail = 0;
	this->head = 0;
	this->threshold = (int) (this->max_size * 0.8);

	int mem_size = this->max_size * sizeof(MemorySpace *);

	this->elements_space = swift_malloc(mem_size);

	this->is_initialized = 0x80012008;

	return true;
}

bool DMLIST::free() {
	//JSFree(this->elements);
	return true;
}
