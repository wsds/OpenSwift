#include "EventLooper.h"

Event * newEvent() {
	MemorySpace * space = swift_malloc(sizeof(Event));
	space->type = TYPE_EVENT;
	Event * event = (Event *) space->pointer;
	event->self = space;
	return event;
}

EventPool * EventPool::instance = NULL;
EventPool * EventPool::getInstance() {
	if (instance == NULL) {
		instance = new EventPool();

	}
	return instance;
}

EventPool::EventPool() {
	this->event_pool = newDMLIST_atomic();
	this->event_id = 0;
	Event * initialize_events[64];
	for (int i = 0; i < 64; i++) {
		initialize_events[i] = this->getEvent();
	}
	for (int i = 0; i < 64; i++) {
		this->freeEvent(initialize_events[i]);
	}
}

Event * EventPool::getEvent() {
	MemorySpace * event_space = this->event_pool->leftPop_atomic();
	if (event_space == NULL) {
		event_space = newEvent()->self;
	}

	Event * event = (Event *) event_space->pointer;
	event->id = this->event_id;
	this->event_id++;
	return event;
}

void EventPool::freeEvent(Event * event) {
	this->event_pool->rightPush_atomic(event->self);
}

EventLooper * EventLooper::instance = NULL;
EventLooper * EventLooper::getInstance() {
	if (instance == NULL) {
		instance = new EventLooper();

	}
	return instance;
}

EventLooper::EventLooper() {

	this->event_queue = newDMLIST_atomic();
	this->numof_event = 0;
	this->status = 0;

	this->pid = getpid();

}

void EventLooper::pushEvent(Event * event) {
	this->event_queue->rightPush_atomic(event->self);
	sigval signal_value;
	signal_value.sival_int = event->id;
	int result = sigqueue(this->pid, 58, signal_value);
}

void EventLooper::loop() {
	this->status = 1006001;
	EventPool * event_pool = EventPool::getInstance();

	Event * event = NULL;
	MemorySpace * event_space = NULL;
	while (true) {
		MemorySpace * event_space = this->event_queue->leftPop_atomic();
		if (event_space == NULL) {
			break;
		}

		event = (Event *) event_space->pointer;
		event->doSomething();
		event_pool->freeEvent(event);
	}
	this->status = 0;
}

void resolveSignal_EventLooper(int signal, siginfo_t * signal_info, void * none) {
	if (signal == 58) {
		EventLooper * eventLooper = EventLooper::getInstance();
		int value = signal_info->si_int;
		if (eventLooper->status == 0) {
			eventLooper->loop();
		}
	} else if (signal == SIGSEGV) {
	}

	return;
}

void EventLooper::regeditSignal() {
	struct sigaction new_sigaction;
	struct sigaction old_sigaction;

	sigemptyset(&new_sigaction.sa_mask);
	new_sigaction.sa_sigaction = this->resolveSignal;
	new_sigaction.sa_flags = SA_SIGINFO;

	sigaction(58, &new_sigaction, &old_sigaction);
	sigaction(SIGTERM, &new_sigaction, &old_sigaction);
}

void EventLooper::threadHolder() {
	this->is_running = true;
	while (this->is_running) {
		sleep(1);
	}
}
