#ifndef EVENTLOOPER_H
#define EVENTLOOPER_H

#include "../baselib/DMLinux.h"
#include "DMLIST_Atomic.h"
//#include <signal.h>
//#include "unistd.h"

class Event {
public:
	MemorySpace * self;

	int id;
	int source;
	int target;
	MemorySpace * data;
	void (*doSomething)();
};

Event * newEvent();

class EventPool {
public:
	static EventPool *instance;
	static EventPool * getInstance();

	EventPool();

	DMLIST_Atomic * event_pool;
	int event_id;

	Event * getEvent();
	void freeEvent(Event * event);
};

class EventLooper {
public:
	static EventLooper *instance;
	static EventLooper * getInstance();

	EventLooper();

	DMLIST_Atomic * event_queue;
	int numof_event;

	int status; //0:sleep  1006001:looping

	void pushEvent(Event * event);
	void loop();

	int pid;
	void (*resolveSignal)(int signal, siginfo_t * signal_info, void * none);
	void regeditSignal();

	bool is_running;
	void threadHolder();
};

#endif /* EVENTLOOPER_H */
