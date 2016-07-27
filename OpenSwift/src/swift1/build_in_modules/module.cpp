#include "module.h"

Module::Module() {
}


void Module::importFunction(NativeFunction *native_function, DMJSON *container) {
    char *definition = native_function->definition;
    int length = getLength(definition);
    int last_index = 0;
    char localChar;

    for (int i = 0; i < length; i++) {
        localChar = definition[i];

        if (localChar == '(') {
            native_function->name = newDMString(0);
            native_function->name->char_string = definition + last_index;
            native_function->name->max_length = i - last_index;
            last_index = i + 1;
        } else if (localChar == ',' || localChar == ')' || localChar == ' ') {
            DMString *param = newDMString(0);
            param->char_string = definition + last_index;
            param->max_length = i - last_index;
            if (param->max_length) {
                native_function->params[native_function->numof_param] = param;
                native_function->numof_param++;
            }
            last_index = i + 1;
        }
    }

    container->set((MemorySpace *) native_function->name, native_function->self);
}
