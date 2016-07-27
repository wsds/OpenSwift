#include "MemoryMap.h"

void backward_memcpy(void *destination, void *source, int count) {
    if (destination == NULL || source == NULL || count <= 0) {
        return;
    }

    char *destination_char = (char *) destination;
    char *source_char = (char *) source;
    for (int i = count - 1; i >= 0; i--) {
        *(destination_char + i) = *(source_char + i);
    }
}

void forward_memcpy(void *destination, void *source, int count) {
    if (destination == NULL || source == NULL || count <= 0) {
        return;
    }

    char *destination_char = (char *) destination;
    char *source_char = (char *) source;
    for (int i = 0; i < count; i++) {
        *(destination_char + i) = *(source_char + i);
    }
}

int getFileLength(int fd) {
    int file_length = -1;
    struct stat fileStat;
    if (-1 == fstat(fd, &fileStat)) {
        file_length = -2;
    } else {
        file_length = fileStat.st_size;
    }
    return file_length;
}

int log2i(int x) {
    //need test.
    x = x | 1;
    int n = 31;
    if (x & 0xffff0000) {
        n -= 16;
        x >>= 16;
    }
    if (x & 0xff00) {
        n -= 8;
        x >>= 8;
    }
    if (x & 0xf0) {
        n -= 4;
        x >>= 4;
    }
    if (x & 0xc) {
        n -= 2;
        x >>= 2;
    }
    if (x & 0x2) {
        n--;
    }

    return 31 - n;
}

int memoryPageAlignment(int length) {
    if (length % 4096 == 0) {
        return length;
    } else {
        return length + 4096 - length % 4096;
    }
}

void setMomoryFolderPath(char *path, int length) {
    momory_folder_path_length = length;
    memset(momory_folder, 0, 64);
    backward_memcpy(momory_folder, path, length);
}

char *getFilePath(char *file_name) {
    int file_name_length = getLength1(file_name);
    backward_memcpy(momory_folder + momory_folder_path_length, file_name, file_name_length);
    *(momory_folder + momory_folder_path_length + file_name_length) = 0;
    return momory_folder;
}

MemorySpace *map_malloc(int size) {
    if (odb_memeory_map == NULL) {
        setupDataMap_ODB();
    }

    MemorySpace *memeorySpace = odb_memeory_map->malloc(size);
    return memeorySpace;
}

void map_free(MemorySpace *space) {
    if (odb_memeory_map == NULL) {
        setupDataMap_ODB();
    }
    odb_memeory_map->free(space);
}

bool map_exchange(MemorySpace *left, MemorySpace *right) {
    if (odb_memeory_map == NULL) {
        setupDataMap_ODB();
    }
    return odb_memeory_map->exchange(left, right);
}

void **get_static_spaces() {
    if (odb_memeory_map == NULL) {
        setupDataMap_ODB();
    }
    return odb_memeory_map->static_spaces;
}

MemorySpace *swift_malloc(int size) {
    if (in_memeory_map == NULL) {
        setupDataMap_IN_MEMORY();
    }

    MemorySpace *memeorySpace = in_memeory_map->malloc(size);
    return memeorySpace;
}

void swift_free(MemorySpace *space) {
    if (in_memeory_map == NULL) {
        setupDataMap_IN_MEMORY();
    }
    in_memeory_map->free(space);
}

bool swift_exchange(MemorySpace *left, MemorySpace *right) {
    if (in_memeory_map == NULL) {
        setupDataMap_IN_MEMORY();
    }
    return in_memeory_map->exchange(left, right);
}

void swift_in_reference(MemorySpace *space) {
    space->reference++;

}

void swift_de_reference(MemorySpace *space) {
    space->reference--;
    if (space->reference <= 0) {
        //todo free the space
    }
}

bool compareMemeorySpace(MemorySpace *left, MemorySpace *right) {

    if (left == right) {
        return true;
    }
    if (left == NULL || right == NULL) {
        return false;
    }
    if (left->length != right->length) {
        return false;
    }

    char *left_pointer = (char *) left->pointer;
    char *right_pointer = (char *) right->pointer;

    if (left_pointer == right_pointer) {
        return true;
    } else if (left->length == 0) {
        return false;
    }

    bool isSame = true;

    for (int i = 0; i < left->length; i++) {
        if (*left_pointer != *right_pointer) {
            isSame = false;
            break;
        }
        left_pointer++;
        right_pointer++;
    }

    return isSame;
}

int getLength1(char *char_string) {
    char *i = char_string;

    while (*i++) {
    }
    int length = (i - char_string - 1);

    return length;
}

void MemeoryMap::initialize() {

    if (this->is_initialized == 600006) {
        return;
    }

    if (this->is_initialized != 0) {
        //error
        this->is_initialized = 0;
        return;
    }
    this->firstload();

    this->is_initialized = 600006;
}

void MemeoryMap::reload() {

}

void MemeoryMap::firstload() {

    this->base = (char *) this;
    this->memory_size = 1024 * 1024 * 16; //*********************memory size*********************
    this->threshold = (int) (this->memory_size * 0.8);
    this->used_size = 0;
    this->memory_base = this->base + 4 * 1024;
    this->swap_page_base = (char *) this->memory_base + (this->memory_size - PAGESIZE64K);

    this->max_page_size = this->memory_size / PAGESIZE64K - 1;
    this->free_page_size = this->max_page_size;
    this->unsorted_page_size = 0;
    this->used_page_size = 0;
    this->pages = (MemoryPage64k *) ((char *) this->memory_base + this->memory_size);
    this->used_page_pool = (int *) ((char *) this->pages + this->max_page_size * sizeof(MemoryPage64k));
    this->free_page_pool = (int *) ((char *) this->pages +
                                    this->max_page_size * (sizeof(MemoryPage64k) + 1 * sizeof(int)));
    for (int i = 0; i < this->free_page_size; i++) {
        *(this->free_page_pool + i) = i;
    }

    this->space_size = 256 * 1024; //*********************pointer size*********************
    this->used_space_size = 1;
    this->used_space_pool = (int *) ((char *) this->spaces + this->space_size * sizeof(MemorySpace));
    this->free_space_pool = (int *) ((char *) this->spaces +
                                     this->space_size * (sizeof(MemorySpace) + 1 * sizeof(int)));
    for (int i = 0; i < this->space_size - this->used_space_size; i++) {
        *(this->free_space_pool + i) = i;
    }

}

void MemeoryMap::finish() {
    if (this->memory_base != NULL && this->memory_fd != -1) {
        int closingFD = this->memory_fd;
        munmap(this->base, this->memory_size);
        close(closingFD);
    }
}

void setupDataMap_ODB() {

    void *base = (void *) 0x41000000;

    char *file_path = getFilePath((char *) "memory.odb");
    int memory_fd = open(file_path, O_CREAT | O_RDWR, 0777);
    int file_length = getFileLength(memory_fd);

    if (file_length < 0) {
        //error
    } else if (file_length == 0) {
        int memory_size = 1024 * 1024 * 16;
        file_length = 4 * 1024 + memory_size +
                      (memory_size / PAGESIZE64K - 1) * (sizeof(MemoryPage64k) + 2 * sizeof(int)); //why minus 1
        file_length = memoryPageAlignment(file_length);
        ftruncate(memory_fd, file_length);
    }

    base = mmap(base, file_length, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, memory_fd, 0);

    char *space_base = (char *) 0x52000000;

    file_path = getFilePath((char *) "space.odb");
    int space_fd = open(file_path, O_CREAT | O_RDWR, 0777);
    file_length = getFileLength(memory_fd);

    if (file_length < 0) {
        //error
    } else if (file_length == 0) {
        int space_size = 256 * 1024;
        file_length = space_size * (sizeof(MemorySpace) + 2 * sizeof(int));
        file_length = memoryPageAlignment(file_length);
        ftruncate(space_fd, file_length);
    }

    space_base = (char *) mmap(space_base, file_length, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, space_fd, 0);

    if (space_base != (void *) 0x52000000 || base != (void *) 0x41000000) {
        //error
    } else {
        MemeoryMap *memeory_map = (MemeoryMap *) base;
        memeory_map->type = TYPE_ODB;
        memeory_map->spaces = (MemorySpace *) space_base;

        memeory_map->memory_fd = memory_fd;
        memeory_map->space_fd = space_fd;

        memeory_map->initialize();
        odb_memeory_map = memeory_map;
    }

}

void setupDataMap_IN_MEMORY() {

    void *base = (void *) 0x61000000;
    int memory_size = 1024 * 1024 * 16;
    int mapping_length =
            4 * 1024 + memory_size + (memory_size / PAGESIZE64K - 1) * (sizeof(MemoryPage64k) + 2 * sizeof(int));
    mapping_length = memoryPageAlignment(mapping_length);
    base = mmap(base, mapping_length, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED | MAP_ANONYMOUS, 0, 0);

    char *space_base = (char *) 0x72000000;
    int space_size = 256 * 1024;
    mapping_length = space_size * (sizeof(MemorySpace) + 2 * sizeof(int));
    mapping_length = memoryPageAlignment(mapping_length);
    space_base = (char *) mmap(space_base, mapping_length, PROT_READ | PROT_WRITE,
                               MAP_SHARED | MAP_FIXED | MAP_ANONYMOUS, 0, 0);

    if (space_base != (void *) 0x72000000 || base != (void *) 0x61000000) {
        //error
    } else {
        MemeoryMap *memeory_map = (MemeoryMap *) base;
        memeory_map->type = TYPE_IN_MEMORY;
        memeory_map->spaces = (MemorySpace *) space_base;
        memeory_map->initialize();
        in_memeory_map = memeory_map;
    }

}

void MemeoryMap::enhanceMemorySize() {

    int old_memory_size = this->memory_size;
    int new_memory_size = old_memory_size * 2;

    int old_mapping_length = 4 * 1024 + old_memory_size +
                             (old_memory_size / PAGESIZE64K - 1) * (sizeof(MemoryPage64k) + 2 * sizeof(int));
    old_mapping_length = memoryPageAlignment(old_mapping_length);

    int new_mapping_length = 4 * 1024 + new_memory_size +
                             (new_memory_size / PAGESIZE64K - 1) * (sizeof(MemoryPage64k) + 2 * sizeof(int));
    new_mapping_length = memoryPageAlignment(new_mapping_length);

    char *base_followed = (char *) this->base + old_mapping_length;

    if (this->type == TYPE_ODB) {
        ftruncate(this->memory_fd, new_mapping_length);
        base_followed = (char *) mmap(base_followed, new_mapping_length - old_mapping_length, PROT_READ | PROT_WRITE,
                                      MAP_SHARED | MAP_FIXED, this->memory_fd, old_mapping_length);
    } else if (this->type == TYPE_IN_MEMORY) {
        base_followed = (char *) mmap(base_followed, new_mapping_length - old_mapping_length, PROT_READ | PROT_WRITE,
                                      MAP_SHARED | MAP_FIXED | MAP_ANONYMOUS, 0, 0);
    } else {
        //error
    }

    //------page--------
    MemoryPage64k *old_pages = this->pages;
    int *old_used_page_pool = this->used_page_pool;
    int *old_free_page_pool = this->free_page_pool;
    int old_page_size = this->max_page_size;

    this->pages = (MemoryPage64k *) ((char *) this->memory_base + new_memory_size);
    this->max_page_size = this->memory_size / PAGESIZE64K - 1;
    this->free_page_size = this->max_page_size;

    this->used_page_pool = (int *) ((char *) this->pages + this->max_page_size * sizeof(MemoryPage64k));
    this->free_page_pool = (int *) ((char *) this->pages +
                                    this->max_page_size * (sizeof(MemoryPage64k) + 1 * sizeof(int)));

    backward_memcpy(this->used_page_pool, old_used_page_pool, this->max_page_size * sizeof(int));
    memset(old_used_page_pool, 0, old_page_size * sizeof(int));

    backward_memcpy(this->free_page_pool, old_free_page_pool, this->max_page_size * sizeof(int));
    memset(old_free_page_pool, 0, old_page_size * sizeof(int));

    backward_memcpy(this->pages, old_pages, this->max_page_size * sizeof(MemoryPage64k));
    memset(old_pages, 0, this->max_page_size * sizeof(MemoryPage64k));
    //------page--------

    //------memory--------
    char *old_swap_page_base = this->swap_page_base;
    this->swap_page_base = (char *) this->memory_base + (new_memory_size - PAGESIZE64K);

    backward_memcpy(this->swap_page_base, old_swap_page_base, PAGESIZE64K);
    memset(old_swap_page_base, 0, PAGESIZE64K);

    this->memory_size = new_memory_size;
    this->threshold = (int) (this->memory_size * 0.8);
    //------memory--------

}

void MemeoryMap::enhenceSpaceSize() {

    int old_space_size = this->space_size;
    int new_space_size = old_space_size + 256 * 1024;

    int old_mapping_length = old_space_size * (sizeof(MemorySpace) + 2 * sizeof(int));
    old_mapping_length = memoryPageAlignment(old_mapping_length);

    int new_mapping_length = new_space_size * (sizeof(MemorySpace) + 2 * sizeof(int));
    new_mapping_length = memoryPageAlignment(new_mapping_length);

    char *space_followed = (char *) this->spaces + old_mapping_length;

    if (this->type == TYPE_ODB) {
        ftruncate(this->space_fd, new_mapping_length);
        space_followed = (char *) mmap(space_followed, new_mapping_length - old_mapping_length, PROT_READ | PROT_WRITE,
                                       MAP_SHARED | MAP_FIXED, this->space_fd, old_mapping_length);
    } else if (this->type == TYPE_IN_MEMORY) {
        space_followed = (char *) mmap(space_followed, new_mapping_length - old_mapping_length, PROT_READ | PROT_WRITE,
                                       MAP_SHARED | MAP_FIXED | MAP_ANONYMOUS, 0, 0);
    } else {
        //error
    }

    int *old_used_space_pool = this->used_space_pool;
    int *old_free_space_pool = this->free_space_pool;

    this->used_space_pool = (int *) ((char *) this->spaces + new_space_size * sizeof(MemorySpace));
    this->free_space_pool = (int *) ((char *) this->spaces + new_space_size * (sizeof(MemorySpace) + 1 * sizeof(int)));

    backward_memcpy(this->used_space_pool, old_used_space_pool, old_space_size * sizeof(int));
    memset(old_used_space_pool, 0, old_space_size * sizeof(int));

    backward_memcpy(this->free_space_pool, old_free_space_pool, old_space_size * sizeof(int));
    memset(old_free_space_pool, 0, old_space_size * sizeof(int));

    this->space_size = new_space_size;

}

