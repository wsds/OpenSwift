#include "SwiftTimer.h"

SwiftTimerCallback * newSwiftTimerCallback() {
	MemorySpace * space = swift_malloc(sizeof(SwiftTimerCallback));
	space->type = TYPE_CALLBACK;
	SwiftTimerCallback * callback = (SwiftTimerCallback *) space->pointer;
	callback->self = space;
	return callback;
}

SwiftTimer * SwiftTimer::instance = NULL;
SwiftTimer * SwiftTimer::getInstance() {
	if (instance == NULL) {
		instance = new SwiftTimer();

	}
	return instance;
}

SwiftTimer::SwiftTimer() {
	this->time_1s = 1000000000;
	this->time_1ms = 1000000;
	this->time_1us = 1000;
	this->time_1ns = 1;

	this->global_id = 10001;
	this->id_space = newDMInt32();

	this->callback_pool = newDMHashTable();
	this->regeditSignal();
}

void resolveSignal_SwiftTimer(int signal, siginfo_t * signal_info, void * none) {
	if (signal == 52) {
		SwiftTimer * swift_timer = SwiftTimer::getInstance();
		int value = signal_info->si_int;
		swift_timer->timeup(value);

	} else if (signal == SIGSEGV) {
	}

	return;
}

int SwiftTimer::setInterval(SwiftTimerCallback * callback, long long time) {

	callback->id_space = newDMInt32();
	callback->id_space->number = this->global_id;
	callback->id = this->global_id;
	this->callback_pool->set((MemorySpace *) callback->id_space, callback->self);
	this->global_id++;

	struct sigevent signal_event;
	memset(&signal_event, 0, sizeof(struct sigevent));
	signal_event.sigev_signo = 52;
	signal_event.sigev_notify = SIGEV_SIGNAL;
	signal_event.sigev_value.sival_int = callback->id;

	if (timer_create(CLOCK_REALTIME, &signal_event, &(callback->linux_timer_id)) == -1) {
		return -1;
	}
	struct itimerspec timer_spec;
	int time_ns = time % this->time_1s;
	int time_s = (time - time_ns) / this->time_1s;
	timer_spec.it_interval.tv_sec = time_s;
	timer_spec.it_interval.tv_nsec = time_ns;
	timer_spec.it_value.tv_sec = time_s;
	timer_spec.it_value.tv_nsec = time_ns;
	if (timer_settime(callback->linux_timer_id, 0, &timer_spec, 0) == -1) {
		int error = linux_errno;
		return -1;
	}
	return callback->id;
}

void SwiftTimer::regeditSignal() {
	struct sigaction new_sigaction;
	struct sigaction old_sigaction;

	sigemptyset(&new_sigaction.sa_mask);
	new_sigaction.sa_sigaction = resolveSignal_SwiftTimer;
	new_sigaction.sa_flags = SA_SIGINFO;

	sigaction(52, &new_sigaction, &old_sigaction);
}

int SwiftTimer::setTimeout(SwiftTimerCallback * callback, long long time) {

	callback->remaining_times = 1;

	callback->id_space = newDMInt32();
	callback->id_space->number = this->global_id;
	callback->id = this->global_id;
	this->callback_pool->set((MemorySpace *) callback->id_space, callback->self);
	this->global_id++;

	struct sigevent signal_event;
	memset(&signal_event, 0, sizeof(struct sigevent));
	signal_event.sigev_signo = 52;
	signal_event.sigev_notify = SIGEV_SIGNAL;
	signal_event.sigev_value.sival_int = callback->id;

	if (timer_create(CLOCK_REALTIME, &signal_event, &(callback->linux_timer_id)) == -1) {
		return -1;
	}
	struct itimerspec timer_spec;
	int time_ns = time % this->time_1s;
	int time_s = (time - time_ns) / this->time_1s;
	timer_spec.it_interval.tv_sec = 0;
	timer_spec.it_interval.tv_nsec = 0;
	timer_spec.it_value.tv_sec = time_s;
	timer_spec.it_value.tv_nsec = time_ns;
	if (timer_settime(callback->linux_timer_id, 0, &timer_spec, 0) == -1) {
		int error = linux_errno;
		return -1;
	}
	return callback->id;
}

void SwiftTimer::clearTimer(int id) {
	this->id_space->number = id;
	DMKeyValue* callback_keyvalue = this->callback_pool->del((MemorySpace *) this->id_space);
	if (callback_keyvalue != NULL) {
		MemorySpace * callback_space = callback_keyvalue->value;
		SwiftTimerCallback * callback = (SwiftTimerCallback *) callback_space->pointer;
		if (timer_delete(callback->linux_timer_id) == -1) {
			return;
		}
		swift_free(callback_keyvalue->key);
		swift_free(callback_keyvalue->value);
		swift_free(callback_keyvalue->self);
	}
}

void SwiftTimer::timeup(int id) {
	this->id_space->number = id;
	MemorySpace * callback_space = this->callback_pool->get((MemorySpace *) this->id_space);
	if (callback_space != NULL) {
		SwiftTimerCallback * callback = (SwiftTimerCallback *) callback_space->pointer;
		if (callback->doSomething != NULL) {
			callback->doSomething();
			callback->remaining_times--;
			if (callback->remaining_times == 0) {
				this->clearTimer(id);
			}
		}
	}
}

