#ifndef DMLIST_ATOMIC_H
#define DMLIST_ATOMIC_H

#include "../DM/DMJSONHelper.h"

class DMLIST_Atomic: public DMLIST {
public:
	void rightPush_atomic(MemorySpace * value);
	MemorySpace * leftPop_atomic();
};
DMLIST_Atomic * newDMLIST_atomic();


#endif /* DMLIST_ATOMIC_H */
