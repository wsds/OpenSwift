#ifndef SWIFTTIMER_H_
#define SWIFTTIMER_H_
#define __USE_POSIX199309

#include "DMLIST_Atomic.h"
#include "../baselib/DMLinux.h"
//#include <time.h>
//#include <signal.h>
//#include <errno.h>

#define CLOCK_REALTIME		0
#define CLOCK_MONOTONIC		1
#define CLOCK_PROCESS_CPUTIME_ID	2
#define CLOCK_THREAD_CPUTIME_ID	3
#define CLOCK_MONOTONIC_RAW		4
#define CLOCK_REALTIME_COARSE	5
#define CLOCK_MONOTONIC_COARSE	6
#define CLOCK_BOOTTIME		7
#define CLOCK_REALTIME_ALARM		8
#define CLOCK_BOOTTIME_ALARM		9

class SwiftTimerCallback {
public:
	MemorySpace * self;

	void (*doSomething)();
	MemorySpace * data;
	int id;
	DMInt32 * id_space;
	void * linux_timer_id;
	int remaining_times;
};
SwiftTimerCallback * newSwiftTimerCallback();

class SwiftTimer {
public:
	static SwiftTimer *instance;
	static SwiftTimer * getInstance();

	SwiftTimer();

	DMHashTable * callback_pool;

	//precision = 0.000 000 001s
	long long time_1s;
	long long time_1ms;
	long long time_1us;
	long long time_1ns;
	int global_id;

	DMInt32 * id_space;

	int setInterval(SwiftTimerCallback * callback, long long time);
	int setTimeout(SwiftTimerCallback * callback, long long time);
	void clearTimer(int id);

	void timeup(int id);
	void regeditSignal();

};

#endif /* SWIFTTIMER_H_ */
