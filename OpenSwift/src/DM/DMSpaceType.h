#ifndef DMSPACETYPE_H
#define DMSPACETYPE_H

#ifndef NULL
#define NULL 0
#endif /* NULL */

/*
 *
 * type summarize:
 * 0xf0 string
 *
 * 0xd0 number
 *
 *
 * 0xe1 list
 * 0xe2 hash table
 * 0xe3 key_value
 * 0xe4 json
 *
 * 0xe5 RootNode
 *
 */


static char TYPE_STRING = 0xf0;

static char TYPE_NUMBER = 0xd0;

static char TYPE_BOOL = 0xb0;

static char TYPE_LIST = 0xe1;
static char TYPE_HASH_TABLE = 0xe2;
static char TYPE_KEY_VALUE = 0xe3;
static char TYPE_JSON = 0xe4;

static char TYPE_ROOT_NODE = 0xe5;
static char TYPE_JSONBASE_CONNECTION_POOL = 0xe6;

static char TYPE_EVENT = 0xd1;
static char TYPE_CALLBACK = 0xd1;

static char TYPE_CODE_KEYWORD = 0xc1;
static char TYPE_CODE_NAME = 0xc2;
static char TYPE_CODE_OPERATOR = 0xc3;

static char TYPE_CONTEXT = 0x91;
static char TYPE_SYNTAX_CHAIN = 0x92;
static char TYPE_OPERATOR_MAP = 0x93;
static char TYPE_NATIVE_OPERATOR_FUNCTION = 0x94;
static char TYPE_NATIVE_FUNCTION = 0x95;
static char TYPE_FUNCTION = 0x96;
static char TYPE_CALLSTACK = 0x97;

#endif /* DMSPACETYPE_H */

