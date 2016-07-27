//
// Created by wsds on 2016/5/25.
//

#ifndef DMLINUX_H
#define DMLINUX_H

#ifndef NULL
#define NULL 0
#endif /* NULL */

//#define MINGW // turn on this or CMAKE_CXX_FLAGS = -DMINGW, if build with MINGW
//#define LINUX_GCC // turn on this or CMAKE_CXX_FLAGS = -DLINUX_GCC, if build with LINUX GCC

#ifdef LINUX_GCC
#include <sys/mman.h>//mmap munmap PROT_READ MAP_SHARED ...
#include <unistd.h>//ftruncate
#include "fcntl.h"//O_CREAT | O_RDWR
#include <sys/stat.h>//fstat
#include <string.h>//mmset memcpy
#include <time.h>
#include <signal.h>
#include "unistd.h"
#include <errno.h>
#endif /* LINUX_GCC */

#ifdef MINGW
//#include <sys/mman.h>
#define PROT_READ    0x1        /* Page can be read.  */
#define PROT_WRITE    0x2        /* Page can be written.  */

#define MAP_SHARED    0x01        /* Share changes.  */
#define MAP_FIXED    0x10        /* Interpret addr exactly.  */
#define MAP_ANONYMOUS    0x20        /* Don't use a file.  */

void *mmap(void *start, int length, int prot, int flags, int fd, int offset);

int munmap(void *start, int length);

//#include <unistd.h>//ftruncate
int ftruncate(int fd, int length);

#include "fcntl.h"//O_CREAT | O_RDWR
#include <sys/stat.h>//fstat

//#include <string.h>//mmset
void *memset(void *point, int value, int size);

// implement it in the assembler, not in the loop of C++ runtime
void memcpy(void *target, const void *source, int size);

//#include <signal.h>
# define si_int _sifields._rt.si_sigval.sival_int
typedef union sigval {
    int sival_int;
    void *sival_ptr;
} sigval_t;

typedef struct siginfo {
    int si_signo;
    /* Signal number.  */
    int si_errno;
    /* If non-zero, an errno value associated with
               this signal, as defined in <errno.h>.  */
    int si_code;
    /* Signal code.  */
    union {
        /* POSIX.1b signals.  */
        struct {
            int si_pid;
            /* Sending process ID.  */
            unsigned int si_uid;
            /* Real user ID of sending process.  */
            sigval_t si_sigval;    /* Signal value.  */
        } _rt;
    } _sifields;
} siginfo_t;

int sigqueue(int pid, int signal, union sigval value);
/* Signals.  */

#define SIGSEGV 11 /* Segmentation violation (ANSI).  */
#define SIGTERM 15 /* Termination (ANSI).  */
#define SA_SIGINFO 4 /* Invoke signal-catching function with three arguments instead of one.  */

# define _SIGSET_NWORDS    (1024 / (8 * sizeof (unsigned long int)))
typedef struct {
    unsigned long int __val[_SIGSET_NWORDS];
} __sigset_t;

struct sigaction {
    void (*sa_sigaction)(int, siginfo_t *, void *);

    int sa_flags;
    __sigset_t sa_mask;
};

int sigemptyset(__sigset_t *__set);

int sigaction(int __sig, struct sigaction *__act, struct sigaction *__oact);

//#include "unistd.h"
unsigned int sleep(unsigned int __seconds);

int usleep(unsigned int __useconds);

int getpid(void);

struct sigevent {
    sigval_t sigev_value;
    int sigev_signo;
    int sigev_notify;
};
enum {
    SIGEV_SIGNAL = 0,        /* Notify via signal.  */
# define SIGEV_SIGNAL    SIGEV_SIGNAL
    SIGEV_NONE,            /* Other notification: meaningless.  */
# define SIGEV_NONE    SIGEV_NONE
    SIGEV_THREAD,            /* Deliver via thread creation.  */
# define SIGEV_THREAD    SIGEV_THREAD

    SIGEV_THREAD_ID = 4        /* Send signal to specific thread.  */
#define SIGEV_THREAD_ID    SIGEV_THREAD_ID
};

//#include <time.h>
struct timespec {
    long int tv_sec;
    /* Seconds.  */
    long int tv_nsec;        /* Nanoseconds.  */
};

struct itimerspec {
    struct timespec it_interval;
    struct timespec it_value;
};

int timer_create(long int clock_id, struct sigevent *evp, void **timerid);

int timer_settime(void *timerid, int flags, struct itimerspec *value, struct itimerspec *ovalue);

int timer_delete(void *timerid);

//#include <errno.h>
int *__errno_location(void);

int lstat(char *file, struct stat *buf);

class DMLinux {

};
#endif /* MINGW */

#define linux_errno (*__errno_location ())
void *JSMalloc(int length);


#endif //DMLINUX_H
