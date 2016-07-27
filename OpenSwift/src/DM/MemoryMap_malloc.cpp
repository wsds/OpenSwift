#include "MemoryMap.h"

void MemeoryMap::refreshPage(MemoryPage64k * page) {
	if (page->spaces_container == NULL) {
		return;
	}

	int * space_indexes = ((int *) page->spaces_container->pointer);
	int space_index;
	MemorySpace * space;

	short j = 0;
	for (short i = 0; i < page->space_length; i++) {
		space_index = *(space_indexes + i);
		if (space_index <= 0) {
			continue;
		}
		space = this->spaces + space_index;

		if (space->length > 0) {
			*(space_indexes + j) = space_index;
			j++;
		} else {
			this->freeSpace(space);
		}
	}

	for (short i = j; i < page->space_length; i++) {
		*(space_indexes + i) = 0;
	}

	page->space_length = j;

}

MemorySpace * MemeoryMap::findAvailableSpace() {
	MemorySpace * space;
	int index = this->free_space_pool[this->used_space_size];
	space = this->spaces + index;
	memset(space, 0, 16);

	this->free_space_pool[this->used_space_size] = -1;
	this->used_space_pool[this->used_space_size] = index;

	this->used_space_size++;

	if (this->used_space_size >= this->space_size) {
		this->enhenceSpaceSize();
	}

	return space;
}

int MemeoryMap::getPageChain(int chain_length) {

	int used = this->used_page_size;
	int size = this->max_page_size;
	int * pool = this->free_page_pool;

	int macthed_chain_length = 1;
	int last_index = -100;
	int index = -100;
	int position = -100;

	if (this->unsorted_page_size > 0) {
		this->sortFreePagePool();
		this->unsorted_page_size = 0;
	}

	for (int i = used; i < size; i++) {
		index = pool[i];

		if (last_index + 1 != index) {
			macthed_chain_length = 1;
		} else {
			macthed_chain_length++;
		}

		if (macthed_chain_length >= chain_length) {
			position = i + 1;
			break;
		}

		last_index = index;
		index = -100;
	}

	if (index >= 0) {
		index = index - chain_length + 1;
		backward_memcpy(pool + used + chain_length, pool + used, (position - chain_length - used) * sizeof(int));

		memset(pool + used, -1, chain_length * sizeof(int));
	}
	MemoryPage64k * page = NULL;
	for (int i = 0; i < chain_length; i++) {
		this->used_page_pool[this->used_page_size + i] = index + i;
		page = this->pages + (index + i);
		page->page_index = (index + i);
		page->state = 600060006;
	}
	this->used_page_size = this->used_page_size + chain_length;
	this->free_page_size = this->free_page_size - chain_length;

	return index;

	//todo resizePage the pages pool when the index get -100
}

MemoryPage64k * MemeoryMap::findAvailablePage(int size, MemorySpace * space) {

	MemoryPage64k * page = NULL;
	int page_index = -1;
	int chain_length = 1;

	if (size > PAGESIZE64K) {

		chain_length = size / PAGESIZE64K + 1;
		if (size % PAGESIZE64K == 0) {
			chain_length--;
		}

		page_index = this->getPageChain(chain_length);

		page = this->pages + page_index;
	} else {
		int power = log2i(size) + 1;
		if (power < 5) {
			power = 5;
		}
		int page_power_index = this->page_power_space[power];
		MemoryPage64k * used_page = NULL;
		int used_page_index = 0;
		bool get_next = false;
		for (int i = page_power_index; i < this->used_page_size; i++) {
			used_page_index = this->used_page_pool[i];
			used_page = this->pages + used_page_index;
			if (used_page->used + size <= PAGESIZE64K) {
				page_index = used_page_index;
				page = used_page;
				page_power_index = i;

				if (page != this->switching_page || get_next == true) {
					break;
				} else {
					page = NULL;
					get_next = true;
				}

			}
		}

		if (page == NULL) {
			page_index = this->getPageChain(1);
			page = this->pages + page_index;
			page_power_index = this->used_page_size - 1;
		}

		if (this->page_power_space[power] != page_power_index) {
			for (int j = 16; j >= power; j--) {
				if (this->page_power_space[j] < page_power_index) {
					this->page_power_space[j] = page_power_index;
				}
			}
		}
	}

	space->length = size;
	space->pointer = this->memory_base + (PAGESIZE64K * (int) (page - this->pages)) + page->used;
	space->index = page->space_length;

	MemoryPage64k * chain_page;
	for (int i = 0; i < chain_length - 1; i++) {
		chain_page = page + i;
		chain_page->used = PAGESIZE64K;
	}
	if (size % PAGESIZE64K != 0) {
		chain_page = page + chain_length - 1;
		chain_page->used = chain_page->used + size % PAGESIZE64K;
		if (chain_length > 1) {
			chain_page->chain_used = chain_page->used;
		}
	}

	if (page_index != -1 && size < PAGESIZE64K && page->spaces_container == NULL) {
		page->max_space_size = 64;
		page->spaces_container = getAvailableSwapSpace(page->max_space_size * sizeof(int));
	}

	this->sortUsedPagePool();

	return page;
}

MemorySpace * MemeoryMap::getAvailableSwapSpace(int size) {
	MemorySpace * space = &(this->swap_spaces[this->swap_space_index]);
	this->swap_space_index = (this->swap_space_index + 1) % 16;
	space->length = size;
	if (this->used_swap_end + size < PAGESIZE64K) {
		space->pointer = this->swap_page_base + this->used_swap_end;
		this->used_swap_end = this->used_swap_end + size;
	} else {
		space->pointer = this->swap_page_base;
		this->used_swap_end = size;
	}

	return space;
}

MemorySpace * MemeoryMap::swapUsedSpace(MemorySpace * space) {

	MemorySpace * new_space = this->malloc(space->length);
	backward_memcpy(new_space->pointer, space->pointer, space->length);
	memset(space->pointer, 0, space->length);

	return new_space;
}

void MemeoryMap::freeUsedSwapSpace(MemorySpace * space) {

	memset(space->pointer, 0, space->length);
}

void MemeoryMap::sortUsedPagePool() { //bubble sort
	int i, j, n, flag = 1;
	int temp;
	int index_left, index_right = 0;

	n = this->used_page_size;
	for (i = 0; i < n - 1 && flag == 1; ++i) {
		flag = 0;
		for (j = 0; j < n - i - 1; ++j) {
			index_left = *(this->used_page_pool + j);
			index_right = *(this->used_page_pool + j + 1);

			if ((this->pages + index_left)->used < (this->pages + index_right)->used) {
				*(this->used_page_pool + j) = index_right;
				*(this->used_page_pool + j + 1) = index_left;
				flag = 1;
			}
		}
	}
}

void MemeoryMap::sortFreePagePool() {
	int i, j, n, flag = 1;
	int temp;
	int index_left, index_right = 0;

	n = this->unsorted_page_size;
	for (i = 0; i < this->unsorted_page_size - 1 && flag == 1; ++i) {
		flag = 0;
		for (j = this->used_page_size; j < this->free_page_size - i - 1; ++j) {
			index_left = *(this->free_page_pool + j);
			index_right = *(this->free_page_pool + j + 1);

			if (index_left > index_right) {
				*(this->free_page_pool + j) = index_right;
				*(this->free_page_pool + j + 1) = index_left;
				flag = 1;
			}
		}
	}
}

void MemeoryMap::transferMemeorySpace(MemorySpace * space) {

	void * old_pointer = space->pointer;
	MemoryPage64k * page = this->findAvailablePage(space->length, space);

	backward_memcpy(space->pointer, old_pointer, space->length);
	memset(old_pointer, 0, space->length);

	if (page->spaces_container != NULL) {
		if (page->space_length + 1 > page->max_space_size) {
			this->enhancePageSpaces(page);
		}

		int * space_indexes = ((int *) page->spaces_container->pointer);
		*(space_indexes + page->space_length) = (int) (space - this->spaces);
		page->space_length++;
	}

	if (page->spaces_container->pointer >= this->swap_page_base) {
		if (page->state != 1006001) {
			page->state = 1006001;
			page->spaces_container = this->swapUsedSpace(page->spaces_container);
			page->state = 600060006;
		}
	}
}

void MemeoryMap::enhancePageSpaces(MemoryPage64k * page) {

	if (this->pages == NULL) {
		return;
	}

	this->refreshPage(page);

	if (page->space_length + 1 > page->max_space_size) {
		MemorySpace * old_spaces = page->spaces_container;

		page->max_space_size = page->max_space_size * 2;
		page->spaces_container = getAvailableSwapSpace(page->max_space_size * sizeof(int));

		backward_memcpy(page->spaces_container->pointer, old_spaces->pointer, old_spaces->length);
		this->free(old_spaces); //todo free the old space in the end
	}
}
MemorySpace * MemeoryMap::malloc(int size) {

	MemorySpace * space = this->findAvailableSpace();
	if (size == 0) {
		space->isZero = IS_ZERO;
		return space;
	}

	if (this->used_size + size > this->threshold) {
		this->enhanceMemorySize();
	}

	MemoryPage64k * page = this->findAvailablePage(size, space);

	if (page->spaces_container != NULL) {
		if (page->space_length + 1 > page->max_space_size) {
			this->enhancePageSpaces(page);
		}

		int * space_indexes = ((int *) page->spaces_container->pointer);
		*(space_indexes + page->space_length) = (int) (space - this->spaces);
		page->space_length++;
	}

	this->used_size = this->used_size + size;

	if (page->spaces_container->pointer >= this->swap_page_base) {
		if (page->state != 1006001) {
			page->state = 1006001;
			page->spaces_container = this->swapUsedSpace(page->spaces_container);
			page->state = 600060006;
		}
	}
	return space;

}

bool MemeoryMap::exchange(MemorySpace * left, MemorySpace * right) {

	int left_page_index = 0;
	int left_space_index = 0;
	if (left->length == 0 || left->type == TYPE_NUMBER) {
		left_page_index = -1;
	} else {
		left_page_index = ((char *) left->pointer - this->memory_base) / PAGESIZE64K;
	}

	int right_page_index = 0;
	int right_space_index = 0;
	if (right->length == 0 || right->type == TYPE_NUMBER) {
		right_page_index = -1;
	} else {
		right_page_index = ((char *) right->pointer - this->memory_base) / PAGESIZE64K;
	}

	if (left_page_index != right_page_index) {

		if (left_page_index != -1) {
			right_space_index = right - this->spaces;
			MemoryPage64k * left_page = this->pages + left_page_index;
			int * left_space_indexes = ((int *) left_page->spaces_container->pointer);
			*(left_space_indexes + left->index) = right_space_index;
		}

		if (right_page_index != -1) {
			left_space_index = left - this->spaces;
			MemoryPage64k * right_page = this->pages + right_page_index;
			int * right_space_indexes = ((int *) right_page->spaces_container->pointer);
			*(right_space_indexes + right->index) = left_space_index;
		}
	}

	int temp[3];
	backward_memcpy(temp, left, 12);
	backward_memcpy(left, right, 12);
	backward_memcpy(right, temp, 12);
	return true;
}

