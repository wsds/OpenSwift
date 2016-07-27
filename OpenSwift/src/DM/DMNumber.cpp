#include "DMNumber.h"

DMInt32 * newDMInt32(){
	DMInt32 * dm_int = (DMInt32 *) swift_malloc(0);//todo get the pointer only but malloc the real space.
	dm_int->type=0xd0;
	dm_int->bit=32;
	return dm_int;
}

void freeDMInt32(DMInt32 * dm_int){
	swift_free((MemorySpace *)dm_int);
}


DMInt64 * newDMInt64(){
	DMInt64 * dm_int = (DMInt64 *) swift_malloc(0);//todo get the pointer only but malloc the real space.
	dm_int->type=0xd0;
	dm_int->bit=64;
	return dm_int;
}

void freeDMInt64(DMInt64 * dm_int){
	swift_free((MemorySpace *)dm_int);
}
