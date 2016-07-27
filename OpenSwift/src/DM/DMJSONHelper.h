#ifndef DMJSONHELPER_H
#define DMJSONHELPER_H

#include "DMJSON.h"
#include "../baselib/temp_string.h"

//
//class DMKeyValueIndicator {
//public:
//	int json_indicator_stack_top;
//	DMKeyValue* dm_key_value;
//};

class DMJSONIndicator {
public:
	int head;
	int tail;
	int quotes_count; //[0,1,3]

	DMJSON* dm_json;
};

DMJSON* parseDMJSON(const char* char_string);

DMJSON* parseDMJSON(char* char_string, int string_length);

DMString* stringifyJSON2DMString(DMJSON* json);


void formatString(DMString * dm_string, MemorySpace * object);

#endif /* DMJSONHELPER_H */

