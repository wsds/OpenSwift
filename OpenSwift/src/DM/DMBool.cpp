#include "DMBool.h"

DMBool * newDMBool() {
	DMBool * dm_bool = (DMBool *) swift_malloc(0); //todo get the pointer only but malloc the real space.
	dm_bool->type = 0xb0;
	return dm_bool;
}

void freeDMBool(DMBool * dm_bool) {
	swift_free((MemorySpace *) dm_bool);
}
