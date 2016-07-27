#include "testSwift.h"

void test16001() {

	Swift * swift = Swift::getInstance();
	int context_id = -1;
	swift->importCode(context_id, "./");
}

void test16000() {
	test16001();
}
