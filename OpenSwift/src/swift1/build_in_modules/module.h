#ifndef MODULE_H
#define MODULE_H

#include "../../DM/DMJSONHelper.h"
#include "../excutor/Context.h"


#ifndef NULL
#define NULL 0
#endif /* NULL */



class Module {
public:
	Module();
	virtual ~Module() {
	}

	virtual void import(DMJSON * import) {
		return;
	}

	void importFunction(NativeFunction *native_function, DMJSON * container);
};


#endif /* MODULE_H */

