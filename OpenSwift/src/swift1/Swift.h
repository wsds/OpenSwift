#ifndef SWIFT_H
#define SWIFT_H

#include "../baselib/DMLinux.h"
#include "SwiftData.h"
#include "Parser.h"
#include "stringifyCode.h"
#include "excutor/Excutor.h"
#include <dirent.h>
#include "build_in_modules/ImportModules.h"

#include <iostream>

class Swift {
public:
	Swift();
	static Swift *instance;
	static Swift * getInstance();

	bool is_initialized;
	bool initialize();

	DMJSON * context_pool;


	void inputCode(int contextID, DMString * code);
	void importCode(int contextID, char * path);
};

#endif /* SWIFT_H */
