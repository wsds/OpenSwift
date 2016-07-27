//
// Created by wsds on 2016/5/25.
//

#include "DMLinux.h"


#ifdef MINGW

void *mmap(void *start, int length, int prot, int flags, int fd, int offset) {
    return nullptr;
}

int munmap(void *start, int length) {
    return 0;
}

int ftruncate(int fd, int length) {
    return 0;
}

void *memset(void *point, int value, int size) {
    return nullptr;
}

void memcpy(void *target, const void *source, int size) {

}

int sigqueue(int pid, int signal, union sigval value) {
    return 0;
}

int sigemptyset(__sigset_t *__set) {
    return 0;
}

int sigaction(int __sig, struct sigaction *__act, struct sigaction *__oact) {
    return 0;
}

unsigned int sleep(unsigned int __seconds) {
    return 0;
}

int usleep(unsigned int __useconds) {
    return 0;
}

int getpid(void) {
    return 0;
}

int timer_create(long int clock_id, struct sigevent *evp, void **timerid) {
    return 0;
}

int timer_settime(void *timerid, int flags, struct itimerspec *value, struct itimerspec *ovalue) {
    return 0;
}

int timer_delete(void *timerid) {
    return 0;
}

int *__errno_location(void) {
    return nullptr;
}

int lstat(char *file, struct stat *buf) {
    return 0;
}

#endif /* MINGW */

void *JSMalloc(int length) {
    return NULL;
}


