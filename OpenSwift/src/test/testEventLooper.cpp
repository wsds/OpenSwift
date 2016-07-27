#include "testEventLooper.h"
using namespace std;

int timer_19002 = 0;
int i_19002 = 0;
void test19002_doSomething() {
	cout << "(0.001s) short time up " << i_19002++ << endl;
	if (i_19002 == 300) {
		SwiftTimer * swift_timer = SwiftTimer::getInstance();
		swift_timer->clearTimer(timer_19002);
	}
}

void startTimer19002() {
	SwiftTimer * swift_timer = SwiftTimer::getInstance();

	SwiftTimerCallback * timer_callback = newSwiftTimerCallback();
	timer_callback->doSomething = &test19002_doSomething;
	timer_19002 = swift_timer->setInterval(timer_callback, 0.001 * swift_timer->time_1s);

}

int timer_19000 = 0;
int i_19000 = 0;
void test19000_doSomething() {
	cout << "(0.5s) time up " << i_19000++ << endl;
	if (i_19000 == 22) {
		startTimer19002();
	}
}

int timer_19001 = 0;
int i_19001 = 0;
void test19001_doSomething() {
	cout << "(5s) long time up " << i_19001++ << endl;
}

void test19001() {

	cout << "setInterval doing" << endl;
	SwiftTimer * swift_timer = SwiftTimer::getInstance();

	SwiftTimerCallback * timer_callback1 = newSwiftTimerCallback();
	timer_callback1->doSomething = &test19001_doSomething;
	timer_19001 = swift_timer->setTimeout(timer_callback1, 5 * swift_timer->time_1s);

	SwiftTimerCallback * timer_callback = newSwiftTimerCallback();
	timer_callback->doSomething = &test19000_doSomething;
	timer_callback->remaining_times = 25;
	timer_19000 = swift_timer->setInterval(timer_callback, 500 * swift_timer->time_1ms);

	cout << "setInterval done" << endl;
//	EventPool * event_pool = EventPool::getInstance();
	EventLooper * event_looper = EventLooper::getInstance();
	event_looper->threadHolder();

}

void test19000() {
	cout << "test19000" << endl; // prints !!!Hello World!!!
	test19001();
}
