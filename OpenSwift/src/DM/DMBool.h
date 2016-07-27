#ifndef DMBOOL_H
#define DMNUMBER_H

#ifndef NULL
#define NULL 0
#endif /* NULL */

#include "MemoryMap.h"

class DMBool {
public:
	char type; //0xb0
	char isZero;
	char flag2;
	char value;// true:0xaa false:0

	MemorySpace * pre_element;

	int empty1;

	short reference;
	short index;
};

/*
 * YES==true==x|(x is number && x!=0)==str|(str is string && str.length>0)==a|(a is JSON && a.length>0)
 * NO==false==x|(x is number && x==0)==str|(str is string && str.length==0)==a|(a is JSON && a.length==0)==null==NULL==undefined
 */


DMBool * newDMBool();
void freeDMBool(DMBool * dm_bool);

#endif /* DMBOOL_H */

