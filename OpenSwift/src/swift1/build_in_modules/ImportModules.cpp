//
// Created by wsds on 2016/6/27.
//

#include "ImportModules.h"


void importModules(DMJSON *globle) {
    Module *module_log = new Module_log();
    module_log->import(globle);
}