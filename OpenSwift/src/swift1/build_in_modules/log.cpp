#include "log.h"


void function_log(MemorySpace *object) {
    DMLog(object);
}

MemorySpace *native_log(DMJSON *local, NativeFunction *native_function) {
    MemorySpace *object = local->get((MemorySpace *) native_function->params[0]);
    function_log(object);
    return NULL;
}


void Module_log::import(DMJSON *import) {
    NativeFunction *native_function = newDMNativeFunction();
    native_function->definition = "log(object)";
    native_function->doSomething = &native_log;
    this->importFunction(native_function, import);
}