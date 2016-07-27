#ifndef TEMP_STRING_H
#define TEMP_STRING_H

#include <string.h>

#ifndef NULL
#define NULL 0
#endif /* NULL */

static char NUMBERCHARSTART = '0';
static char CHAR_minus = '-';
static char NUMBERCHAREND = '9';

static char SINGLEQUOTE = '\'';
static char DOUBLEQUOTES = '"';
static char EQUALITY = '=';
static char BLANK = ' ';

static char SLASH = '/';

static char COMMA = ',';
static char SEMICOLON = ';';
static char COLON = ':';

static char LEFTSMALLBRACKET = '(';
static char RIGHTSMALLBRACKET = ')';

static char LEFTBRACKET = '[';
static char RIGHTBRACKET = ']';

static char LEFTBIGBRACKET = '{';
static char RIGHTBIGBRACKET = '}';

static char BR = '\n';
static char ENTER = '\r';

static char STREND = '\0';

static char TAB = '\t';

int strlen(char *str);
int strlen(char *str, char * keyEnd);
int strcmp(char *src, char *dst);
int strcmp(char *src, char *dst, char * keyEnd);
int strcopy(char *source, char *target);
int strcopy(char *source, char *target, int length);
int strappend(char *target, char *source);
void strclear(char *str);

bool isNumber(char *string);
bool isNumber(char *string, int length);
bool isNumberHex(char *string, int length);
int parseStringToNubmer(char* string, int length);
int parseNubmerToString(int number, char * target);
int parseNubmerToString(long number, char * target);

bool isJSON(char *string, int length);

static char NUMBER_CHAR_START = '0';
static char NUMBER_CHAR_END = '9';
static char CHAR_AT = '@';
static char number_string_buffer[16] = "";

int getLength(char *char_string);
int getLength(char *char_string, char * keyEnd);

bool stringCompare(char *left, char *right);
bool stringCompare(char *left, char *right, char * keyEnd);
bool stringCompare(char *left, char *right, int length);
bool stringCompare(void *left, void *right, int length);

static char buf[16] = ""; //need memory optimize
static int radix = 10;
int numberToString(int number, char * char_string);

void reverse_memcpy(void * destination, void * source, int count);

void obverse_memcpy(void * destination, void * source, int count);
void obverse_memcpy_slow(void * destination, void * source, int count);

static const char* funcStr = "\"FUNCTION\"";
static const char* classStr = "\"CLASS\"";
static const char* nullStr = "\"NULL\"";

static bool HasKeyQuote = false;
static bool useBIGBRACKET = false;

#endif /* TEMP_STRING_H */
