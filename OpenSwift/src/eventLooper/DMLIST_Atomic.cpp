#include "DMLIST_Atomic.h"

void DMLIST_Atomic::rightPush_atomic(MemorySpace * value) {
	int tail_atomic = this->tail;
	this->tail++;
	this->length++;

	MemorySpace ** elements = (MemorySpace **) this->elements_space->pointer;
	int element_index = tail_atomic % this->max_size;
	element_index = (element_index + this->max_size) % this->max_size;

	elements[element_index] = value;

}

MemorySpace * DMLIST_Atomic::leftPop_atomic() {

	this->head++;
	this->length--;

	int index = 0;
	MemorySpace ** elements = (MemorySpace **) this->elements_space->pointer;
	int element_index = (this->head + index) % this->max_size;
	element_index = (element_index + this->max_size) % this->max_size;

	MemorySpace * old_value = elements[element_index];
	elements[element_index] = NULL;
	return old_value;
}

DMLIST_Atomic * newDMLIST_atomic() {
	MemorySpace * space = swift_malloc(sizeof(DMLIST_Atomic));
	space->type = TYPE_LIST;
	DMLIST_Atomic * dm_list = (DMLIST_Atomic *) space->pointer;
	dm_list->self = space;
	dm_list->initialize();
	return dm_list;
}
