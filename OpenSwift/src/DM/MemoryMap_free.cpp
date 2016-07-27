#include "MemoryMap.h"

void MemeoryMap::freeSpace(MemorySpace * space) {
	if (space == NULL) {
		return;
	}

	memset(space, 0, 16);

	int space_index = space - this->spaces;
	if (space_index == 0) {
		return;
	}
	this->free_space_pool[this->used_space_size - 1] = space_index;
	this->used_space_pool[this->used_space_size - 1] = -1;

	this->used_space_size--;
}

/*
 * 为了保证代码的可读性，需要对幂律指针序列的作用机制进行补充说明
 *
 * todo
 *
 */
void MemeoryMap::freePage(MemoryPage64k * page) {

	if (page->state != 600060006) {
		return;
	}
	page->state = -1000;

	int old_page_index = page->page_index;
	memset(page, 0, sizeof(MemoryPage64k));

	this->free_page_pool[this->used_page_size - 1] = old_page_index;

	bool need_move = false;
	int used_page_pool_index = 0;
	for (int i = 0; i < this->used_page_size; ++i) {
		if (need_move == true) {
			this->used_page_pool[i - 1] = this->used_page_pool[i];
		}
		if (this->used_page_pool[i] == old_page_index) {
			used_page_pool_index = i;
			need_move = true;
		}
	}

	this->used_page_pool[this->used_page_size - 1] = 0;

	if (need_move == true) {
		for (int j = 16; j >= 5; j--) {
			if (this->page_power_space[j] > used_page_pool_index) {
				this->page_power_space[j]--;
			}
		}
	}

	this->used_page_size--;
	this->free_page_size++;

	this->unsorted_page_size++;

}

void MemeoryMap::switchPage(MemoryPage64k * page) {
	if (page->state != 600060006) {
		return;
	}

	if (page->spaces_container == NULL) {
		return;
	}

	int * space_indexes = NULL;
	MemorySpace * temp_spaces_container = NULL;
	int spaces_page_index = ((char *) page->spaces_container->pointer - this->memory_base) / PAGESIZE64K;
	if (spaces_page_index == page->page_index) {
		if (page->space_length == 1) {
			this->freeSpace(page->spaces_container);
			this->freePage(page);
			return;
		}

		temp_spaces_container = this->getAvailableSwapSpace(page->spaces_container->length);
		backward_memcpy(temp_spaces_container->pointer, page->spaces_container->pointer, page->spaces_container->length);
		space_indexes = ((int *) temp_spaces_container->pointer);

	} else {
		space_indexes = ((int *) page->spaces_container->pointer);
	}

	int space_index;
	MemorySpace * space;

	this->switching_page = page;
	for (short i = 0; i < page->space_length; i++) {
		space_index = *(space_indexes + i);
		if (space_index <= 0) {
			continue;
		}
		space = this->spaces + space_index;
		if (space == page->spaces_container) {
			this->freeSpace(space);
		}
		if (space->length > 0) {
			this->transferMemeorySpace(space);
		} else {
			this->freeSpace(space);
		}

		*(space_indexes + i) = -1;
	}
	this->switching_page = NULL;

	MemorySpace * old_spaces_container = NULL;
	if (spaces_page_index == page->page_index) {
		this->freeUsedSwapSpace(temp_spaces_container);
	} else {
		old_spaces_container = page->spaces_container;
	}
	page->spaces_container = NULL;

	this->freePage(page);

	if (old_spaces_container != NULL) {
		this->free(page->spaces_container);
	}

}

/*
 * 为了保证代码的可读性，必须有下面的注释
 *
 * 下面的函数的目的是保证栈式使用小内存块的高效率，做法是这样的：
 * 1. 只遍历256个page的space索引空间，并且把超过10个的空置区去掉
 * 2. 如果后面的索引空间都是空置区，则调整page的used和free结构，将空置区所对应内存划归可用资源。
 */
void MemeoryMap::freePageSpaceIndexes(MemoryPage64k * page, MemorySpace * space) {

	short free_index = space->index;
	if (free_index >= page->space_length || page->spaces_container == NULL) {
		return;
	}

	if (page->space_length - free_index >= 256) {
		int * space_indexes = ((int *) page->spaces_container->pointer);
		*(space_indexes + free_index) = -1;
		return;
	}

	int * space_indexes = ((int *) page->spaces_container->pointer);
	*(space_indexes + free_index) = -1;

	int numof_continuous_free_space = 0;
	for (short index = free_index; index < page->space_length; index++) {

		if (*(space_indexes + index) != -1) {
			if (numof_continuous_free_space > 10) {
				forward_memcpy(space_indexes + free_index, space_indexes + index, numof_continuous_free_space * 4);
				memset(space_indexes + (page->space_length - numof_continuous_free_space), 0, numof_continuous_free_space * 4);
				page->space_length = page->space_length - numof_continuous_free_space;
			}

			break;
		}
		numof_continuous_free_space++;

		if (index == page->space_length - 1) {
			memset(space_indexes + (page->space_length - numof_continuous_free_space), 0, numof_continuous_free_space * 4);
			page->space_length = page->space_length - numof_continuous_free_space;

			int page_index = page->page_index;
			int stack_offset = (char *) space->pointer - (this->memory_base + PAGESIZE64K * page_index) - 1;

			page->freed = page->freed - (page->used - stack_offset);
			page->used = stack_offset;
		}
	}
}

void MemeoryMap::free(MemorySpace * space) {

	if (this->is_freeing == true) {
		this->freeing_spaces[this->freeing_space_size] = space;
		this->freeing_space_size++;
		return;
	}

	if (space->length == 0 || space->isZero == IS_ZERO) {
		this->freeSpace(space);
		return;
	}

	this->is_freeing = true;

	//todo check whether the space is in the scope of the maintaining spaces
	int page_index = ((char *) space->pointer - this->memory_base) / PAGESIZE64K;
	int page_offset = (char *) space->pointer - (this->memory_base + PAGESIZE64K * page_index);
	MemoryPage64k * page = this->pages + page_index;

	int chain_length = space->length / PAGESIZE64K + 1;
	MemoryPage64k * chain_page;
	for (int i = 0; i < chain_length - 1; i++) {
		chain_page = page + i;
		this->freePage(chain_page);
	}

	if (space->length % PAGESIZE64K != 0) {
		chain_page = page + chain_length - 1;
		if (page->state == 600060006) {
			if (page_offset + space->length == chain_page->used) {
				chain_page->used = chain_page->used - space->length % PAGESIZE64K;
			} else {
				chain_page->freed = chain_page->freed + space->length % PAGESIZE64K;
			}
			if (chain_length > 1) {
				chain_page->chain_used = 0;
				page = chain_page;
			}
		}
	}

	this->freePageSpaceIndexes(page, space);

	int space_length = space->length;
	void * space_pointer = space->pointer;

	this->freeSpace(space);

	if (page->used == page->freed) {
		this->switchPage(chain_page);
	} else if (page->freed > (PAGESIZE64K / 2) && page->chain_used == 0) {
		this->switchPage(page);
	} else if (page->spaces_container != NULL && page->used == page->freed + page->spaces_container->length) {
		int spaces_page_index = ((char *) page->spaces_container->pointer - this->memory_base) / PAGESIZE64K;
		if (spaces_page_index == page->page_index) {
			this->switchPage(page);
		}
	}

	if (space_pointer != NULL && space_length != 0) {
		memset(space_pointer, 0, space_length);
	}

	MemorySpace * freeing_space = NULL;
	if (this->freeing_space_size > 0) {
		this->freeing_space_size--;
		freeing_space = this->freeing_spaces[this->freeing_space_size];
		this->freeing_spaces[this->freeing_space_size] = NULL;
	}
	this->is_freeing = false;

	if (freeing_space != NULL) {
		this->free(freeing_space);
	}

}
