#ifndef STRINGIFYCODE_H
#define STRINGIFYCODE_H

#include "SwiftData.h"

#ifndef NULL
#define NULL 0
#endif /* NULL */

void stringifyMeta(MemorySpace * meta, int level, DMString * code_string);
void stringifyCode(DMJSON * code_block, int level, DMString * code_string);

DMString * stringifyCode(DMJSON * code_block);

#endif /* STRINGIFYCODE_H */

