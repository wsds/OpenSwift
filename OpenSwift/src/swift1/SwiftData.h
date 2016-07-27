#ifndef SWIFTDATA_H
#define SWIFTDATA_H

#include "../eventLooper/DMLIST_Atomic.h"

class JSONPool {
public:
	static JSONPool *instance;
	static JSONPool * getInstance();

	JSONPool();

	DMLIST_Atomic * json_pool;
	int json_id;

	DMJSON * getDMJSON();
	void freeDMJSON(DMJSON * json);
};


class SwiftData {
public:
	SwiftData();
};

class CodeName{
public:
	char type; //0xc2
	char isZero;
	char nameType;// 0xff:var  0xee:local  0xdd:FDV 0xcc:FCV  0xbb:in_class  0xaa:in_expression 0x99:JSON_key 0x88:Function_name 0x77:Class_name 0x66:RV
	char lookup_id; //the resolved CodeName cached in the resolved_pool(size is only 256-1) in the context.
	char * char_string;
	int max_length;
	short reference;
	short index;
};

//DMString
//DMNumber
class KeyWord{
public:
	char type; //0xc1
	char isZero;
	short used_length;
	char * char_string;
	int max_length;
	short reference;
	short index;
};
//Operator
//KeyWords


#endif /* SWIFTDATA_H */
