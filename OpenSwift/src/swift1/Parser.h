#ifndef PARSER_H
#define PARSER_H
#include "SwiftData.h"



class ParserHelper {
public:

	static ParserHelper *instance;

	static ParserHelper * getInstance();

	ParserHelper();
	bool is_initialized;
	void initialize();
	void addKeyWords(char * keyWordStr);
	void addOperators(char * operatorStr);

	MemorySpace * mark;
	DMJSON * keyWordMap;
	DMString * operatorSet;

	bool isKeyWord(DMString* name);
	bool isOperator(char localChar);

	DMString * no_name;

};

static int NOT_IN_WORD = 0;
static int IN_WORD = 0x61;
static int NOT_IN_LINE = 0;
static int IN_LINE = 0x61;

class CodeIndicator {
public:
	int parse_status; //[0:root,11:{JSON},12:{class},13:{function},14:{block},21:[json],22:[enum],23:[key|index],31:(FDV),32:(function call),33:(return),34:(expression),35:(condition),41:"string"，51: @operator@], 61: //comments, 62:/*comments*/
	int start;
	int end;
	int flag;

	DMJSON * code;

//	DMJSON * lastLine;

	MemorySpace * last_meta;
	DMJSON * includer;
	DMKeyValue * key_value;

	int isInLine;//0:NOT_IN_LINE 0x61: IN_LINE
	int isInWord;//0:NOT_IN_WORD; 0x61: IN_WORD
};

class Parser {
public:

	static Parser *instance;
	static Parser * getInstance();

	Parser();
	bool is_initialized;
	void initialize();

	ParserHelper * helper;
	JSONPool * json_pool;

	DMJSON* parseCode(char* char_string, int string_length);
	void pushMeta(MemorySpace * meta, CodeIndicator * indicator);
	MemorySpace * resolveExpression(DMJSON * expression);
};

#endif /* PARSER_H_ */
