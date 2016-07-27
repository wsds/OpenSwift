#include "Parser.h"

Parser * Parser::instance = NULL;
Parser * Parser::getInstance() {
	if (instance == NULL) {
		instance = new Parser();
	}
	return instance;
}

Parser::Parser() {
	this->is_initialized = false;
	this->initialize();
}

void Parser::initialize() {
	this->helper = ParserHelper::getInstance();
	this->json_pool = JSONPool::getInstance();

	this->is_initialized = true;
}

ParserHelper * ParserHelper::instance = NULL;
ParserHelper * ParserHelper::getInstance() {
	if (instance == NULL) {
		instance = new ParserHelper();
	}
	return instance;
}
ParserHelper::ParserHelper() {

	this->keyWordMap = NULL;
	this->is_initialized = false;

}

void ParserHelper::addKeyWords(char * keyWordStr) {
	int length = getLength(keyWordStr);
	int lastIndex = 0;
	char c;
	for (int i = 0; i < length + 1; i++) {
		c = keyWordStr[i];
		if (c == '|' || c == 0) {
			DMString * key = newDMString(0);
			key->char_string = keyWordStr + lastIndex;
			key->max_length = i - lastIndex;

			this->keyWordMap->set((MemorySpace *) key, this->mark);
			lastIndex = i + 1;
		}
	}
}

void ParserHelper::addOperators(char * operatorStr) {
	*this->operatorSet * operatorStr;
}

void ParserHelper::initialize() {

	this->keyWordMap = newDMJSON();
	this->mark = (MemorySpace *) newDMString(0);

	this->addKeyWords("var|local|new");
	this->addKeyWords("if|else|for|in|do|while|break|continue");
	this->addKeyWords("func|function|callback|return|async");
	this->addKeyWords("in|with|is|as|has");
	this->addKeyWords("class|extends|this|self");
	this->addKeyWords("import|from");

	this->operatorSet = newDMString(32);
	this->addOperators("=+-*/");
	this->addOperators("~!@#$%^*");
	this->addOperators("<>|&?");

	this->no_name = newDMString(8);
	*this->no_name * "NO_NAME";

	this->is_initialized = true;
}

bool ParserHelper::isKeyWord(DMString* name) {
	if (this->is_initialized == false) {
		this->initialize();
	}

	MemorySpace * mark = this->keyWordMap->get(name);
	if (mark == NULL) {
		return false;
	} else {
		return true;
	}
}

bool ParserHelper::isOperator(char localChar) {
	if (this->is_initialized == false) {
		this->initialize();
	}

	for (int i = 0; i < this->operatorSet->max_length; i++) {
		if (localChar == *(this->operatorSet->char_string + i)) {
			return true;
		}
	}
	return false;
}

bool isOperator(char localChar) {
	ParserHelper * keyWords = ParserHelper::getInstance();
	return keyWords->isOperator(localChar);
}

bool isKeyWord(DMString* name) {
	ParserHelper * keyWords = ParserHelper::getInstance();
	return keyWords->isKeyWord(name);
}

bool isBlank(char localChar) {
	if (localChar == ' ' || localChar == '\t' || localChar == '\v') {
		return true;
	}
//	if (localChar == '\n' || localChar == '\r' || localChar == '\0') {
//		return true;
//	}
	return false;
}
bool isBracket(char localChar) {
	if (localChar == '(' || localChar == ')' || localChar == '[' || localChar == ']' || localChar == '{' || localChar == '}') {
		return true;
	}
	return false;
}

bool isLetter(char localChar) {
	if (localChar >= 'a' && localChar <= 'z') {
		return true;
	}
	if (localChar >= 'A' && localChar <= 'Z') {
		return true;
	}
	if (localChar >= '0' && localChar <= '9') {
		return true;
	}
	if (localChar == '_' || localChar == '.') {
		return true;
	}
	return false;
}

bool isSeparator(char localChar) {
	if (localChar == ';' || localChar == ',') {
		return true;
	}
	return false;
}

bool isLineEnd(char localChar) {
	if (localChar == '\n' || localChar == '\r' || localChar == '\0') {
		return true;
	}
	return false;
}

#define indicator_stack_size  20
int indicator_stack_top = 0;
CodeIndicator indicator_stack[indicator_stack_size];

int checkString(char preChar, char localChar, char nextChar) {
	if (localChar == '\'' && nextChar == '\'' && preChar == '\'') {
		return 13;
	} else if (localChar == '"' && nextChar == '"' && preChar == '"') {
		return 23;
	} else if (localChar == '"' && nextChar != '"' && preChar != '"') {
		return 21;
	} else if (localChar == '\'' && nextChar != '\'' && preChar != '\'') {
		return 11;
	} else if (localChar == '"' && nextChar != '"' && preChar == '"') {
		return 22;
	} else if (localChar == '\'' && nextChar != '\'' && preChar == '\'') {
		return 12;
	}
	return 0;
}

DMString * parseDMString(char * string, CodeIndicator * indicator) {
	char *tartget_string = string + indicator->start;
	int length = indicator->end - indicator->start;
	DMString * dm_string = newDMString(0);
	dm_string->char_string = tartget_string;
	dm_string->used_length = length;
	dm_string->max_length = length;

	return dm_string;
}

MemorySpace * parseElement(char * string, int start, int end) {
	MemorySpace * element = NULL;

	char *tartget_string = string + start;
	int length = end - start;
	if (length == 0) {
		return NULL;
	}
	if (isNumber(tartget_string, length)) {
		DMInt32 * int32 = newDMInt32();
		int32->number = parseStringToNubmer(tartget_string, length);
		element = (MemorySpace *) int32;

	} else if (isNumberHex(tartget_string, length)) {
		DMInt64 * int64 = newDMInt64();
		int64->number = parseStringToNubmer(tartget_string, length); //todo
		element = (MemorySpace *) int64;
	} else {
		DMString* name = newDMString(0);
		name->char_string = tartget_string;
		name->used_length = length;
		name->max_length = length;

		if (isKeyWord(name)) {
			name->type = TYPE_CODE_KEYWORD;
		} else {
			name->type = TYPE_CODE_NAME;
		}

		element = (MemorySpace *) name;
	}

	return element;
}
DMString* spliteFunctionName(DMString* name) {
	DMString* function_name = newDMString(0);
	char localChar = 0;
	for (int i = name->max_length - 1; i >= 0; i--) {
		localChar = name->char_string[i];
		if (localChar == '.') {
			function_name->char_string = name->char_string + i + 1;
			function_name->used_length = name->max_length - i - 1;
			function_name->max_length = name->max_length - i - 1;
			break;
		}
	}
	if (function_name->max_length == 0) {
		function_name->char_string = name->char_string;
		function_name->used_length = name->max_length;
		function_name->max_length = name->max_length;
	}
	return function_name;
}

void clearCodeIndicator(CodeIndicator * indicator) {
	memset(indicator, 0, sizeof(CodeIndicator));
}

MemorySpace * Parser::resolveExpression(DMJSON * expression) {
	if (expression->type != 80) {
		return NULL;
	}

	if (expression->dm_list->length > 1) {
		MemorySpace * first_element = expression->get(0, NOT_DEL);
		if (first_element->type == TYPE_CODE_KEYWORD) {
			DMString * keyword = (DMString *) expression->get(0, DEL);
			if (*keyword == "import") {
				expression->type = 51;
			} else if (*keyword == "return") {
				expression->type = 33;
			}
		} else {
			expression->type = 10;
		}

		return expression->self;
	}

	MemorySpace * first_element = expression->get(-1, DEL);
	if (expression->dm_list->length != 0) {
		//error
		return NULL;
	}

	this->json_pool->freeDMJSON(expression);
	return first_element;
}

void Parser::pushMeta(MemorySpace * meta, CodeIndicator * indicator) {
	if (meta == NULL) {
		return;
	}

	MemorySpace * last_meta = indicator->last_meta;
	indicator->last_meta = meta;

	if (meta->type == TYPE_CODE_KEYWORD) {
		DMString * keyword = (DMString *) meta;
		if (indicator->includer == NULL) {
			if (*keyword == "func" || *keyword == "function" || *keyword == "callback") {
				DMJSON * functionDefinition = this->json_pool->getDMJSON();
				functionDefinition->type = 30;
				if (indicator->parse_status == 32 || indicator->parse_status == 81 ||indicator->parse_status == 33) {
					DMJSON* includer = this->json_pool->getDMJSON();
					includer->type = 80;
					indicator->includer = includer;
					indicator->includer->set(-1, functionDefinition->self, NOT_REPLACE);
				} else {
					indicator->includer = functionDefinition;
				}
			} else if (*keyword == "class") {
				DMJSON* classDefinition = this->json_pool->getDMJSON();
				classDefinition->type = 20;
				indicator->includer = classDefinition;
			} else if (*keyword == "if") {
				DMJSON* ifBlock = this->json_pool->getDMJSON();
				ifBlock->type = 41;
				indicator->includer = ifBlock;
			} else if (*keyword == "else") {
				DMJSON* else_ifBlock = this->json_pool->getDMJSON();
				else_ifBlock->type = 45;
				indicator->includer = else_ifBlock;
			} else if (*keyword == "for") {
				DMJSON* forBlock = this->json_pool->getDMJSON();
				forBlock->type = 42;
				indicator->includer = forBlock;
			} else if (*keyword == "while") {
				DMJSON* whileBlock = this->json_pool->getDMJSON();
				whileBlock->type = 44;
				indicator->includer = whileBlock;
			} else if (*keyword == "return") {
				DMJSON* functionReturn = this->json_pool->getDMJSON();
				functionReturn->type = 33;
				indicator->includer = functionReturn;
//				indicator->includer->set(-1, meta, NOT_REPLACE);
			} else if (*keyword == "import") {
				DMJSON* import = this->json_pool->getDMJSON();
				import->type = 80;
				indicator->includer = import;
				indicator->includer->set(-1, meta, NOT_REPLACE);
			} else if (*keyword == "break" || *keyword == "continue") {
				DMJSON* includer = this->json_pool->getDMJSON();
				includer->type = 80;
				indicator->includer = includer;
				indicator->includer->set(-1, meta, NOT_REPLACE);
			}
		} else {
			if (indicator->includer->type == 80) {
				if (*keyword == "func" || *keyword == "function" || *keyword == "callback") {
					DMJSON * functionDefinition = this->json_pool->getDMJSON();
					functionDefinition->type = 30;
//					indicator->includer = functionDefinition;
					indicator->includer->set(-1, functionDefinition->self, NOT_REPLACE);
				} else if (*keyword == "in" || *keyword == "has" || *keyword == "is" || *keyword == "as") {
					indicator->includer->set(-1, meta, NOT_REPLACE);
				} else if (*keyword == "from") {
					indicator->includer->set(-1, meta, NOT_REPLACE);
				}
			} else if (indicator->includer->type == 45) {
				if (*keyword == "if") {
				}
			}
		}

	} else {
		if (indicator->includer == NULL) {
			DMJSON* includer = this->json_pool->getDMJSON();
			includer->type = 80;
			indicator->includer = includer;
		}

		if (meta->type == TYPE_CODE_NAME) {
			CodeName * code_name = (CodeName *) meta;

			if (last_meta != NULL && last_meta->type == TYPE_CODE_KEYWORD) {
				DMString * last_keyword = (DMString *) last_meta;
				if (*last_keyword == "var") {
					if (indicator->parse_status == 12) {
						code_name->nameType = 0xbb;
					} else if (indicator->parse_status == 13 || indicator->parse_status == 14) {
						code_name->nameType = 0xff;
					}
				} else if (*last_keyword == "local") {
					code_name->nameType = 0xee;
				} else if (*last_keyword == "func" || *last_keyword == "function" || *last_keyword == "callback") {
					if (indicator->includer->type == 30) {
						code_name->nameType = 0x88;
						if (*last_keyword == "callback") {
							code_name->nameType = 0x89;
						}

						DMJSON* functionDefinition = indicator->includer;
						if (functionDefinition->type != 30 && functionDefinition->name != NULL) {
							//syntax error
							return;
						}
						functionDefinition->name = (DMString *) code_name;
						return;
					}
				} else if (*last_keyword == "class") {
					if (indicator->includer->type == 20) {
						code_name->nameType = 0x77;

						DMJSON* classDefinition = indicator->includer;
						if (classDefinition->type != 20 && classDefinition->name != NULL) {
							//syntax error
							return;
						}
						classDefinition->name = (DMString *) code_name;
						return;

					}
				} else if (*last_keyword == "extends") {
					if (indicator->includer->type == 20) {
						code_name->nameType = 0x76;

						DMJSON* classDefinition = indicator->includer;
						if (classDefinition->type != 20 && classDefinition->parent_name != NULL) {
							//syntax error
							return;
						}
						classDefinition->parent_name = (DMString *) code_name;
						return;

					}
				} else if (*last_keyword == "new") {
					code_name->nameType = 0x82;
				} else if (*last_keyword == "async") {
					code_name->nameType = 0x83;
				}
			} else {
				if (indicator->parse_status == 11 || indicator->parse_status == 21) {
					code_name->nameType = 0x99;
				} else if (indicator->parse_status == 14 || indicator->parse_status == 11 || indicator->parse_status == 12 || indicator->parse_status == 13) {
					if (indicator->includer->type == 20) {
						code_name->nameType = 0x77;
					} else if (indicator->includer->type == 30) {
						code_name->nameType = 0x88;
					} else if (indicator->includer->type == 33) {
						code_name->nameType = 0x66;
					} else {
						code_name->nameType = 0xaa;
					}
				} else if (indicator->parse_status == 31) {
					code_name->nameType = 0xdd;
				} else if (indicator->parse_status == 32) {
					code_name->nameType = 0xcc;
				} else if (indicator->parse_status == 33) {
					code_name->nameType = 0x66;
				} else {
					code_name->nameType = 0xaa;
				}
			}

		} else if (meta->type == TYPE_CODE_OPERATOR) {

		} else if (meta->type == TYPE_STRING) {

		} else if (meta->type == TYPE_NUMBER) {

		} else if (meta->type == TYPE_JSON) {

		}
		indicator->includer->set(-1, meta, NOT_REPLACE);
	}
}

void pushElement(MemorySpace * element, CodeIndicator * indicator) {
	if (indicator->key_value != NULL) {
		DMKeyValue * key_value = indicator->key_value;
		key_value->value = element;
	} else if (indicator->code != NULL) {
		indicator->code->set(-1, element, NOT_REPLACE);
	} else if (indicator->includer != NULL) {
		indicator->includer->set(-1, element, NOT_REPLACE);
	}
}

DMJSON * Parser::parseCode(char* code_char_string, int string_length) {

	indicator_stack_top = 0;
	CodeIndicator * indicator = indicator_stack + indicator_stack_top; //current indicator
	DMJSON * root = this->json_pool->getDMJSON();
	root->type = 34;
	indicator->code = root;
	indicator->parse_status = 14;

	DMString * code_string = newDMString(string_length);
	code_string->copy(code_char_string, string_length);
	char* char_string = code_string->char_string;

	char preChar;
	char localChar = 0;
	char nextChar;
	int last_index = 0;
	int i = -1;

	int line_number = 0;
	while (true) {
		if (i >= string_length) {
			break;
		} else {
			i++;
		}

		if (last_index > i) {
			continue;
		}
		preChar = localChar;
		localChar = char_string[i];
		nextChar = char_string[i + 1];
		if (localChar == '\n') {
			line_number++;
		}

		if (indicator->parse_status == 51) { //@operator@
			if (isOperator(localChar)) {
				continue;
			}

			indicator->end = i;

			DMString * dm_string = parseDMString(char_string, indicator);
			dm_string->type = TYPE_CODE_OPERATOR;

			clearCodeIndicator(indicator);
			indicator_stack_top--;
			indicator = indicator_stack + indicator_stack_top;

			pushMeta((MemorySpace *) dm_string, indicator);

			last_index = i;
		}

		if (indicator->parse_status == 41) { //"string"
			int string_state = checkString(preChar, localChar, nextChar);

			if (string_state == 0 || indicator->flag != string_state || indicator->start + 1 > i) {
				continue;
			}

			if (string_state % 10 == 1) {
				indicator->end = i;
				last_index = i + 1;
			} else if (string_state % 10 == 3) {
				indicator->end = i - 1;
				last_index = i + 2;
			}
			DMString * dm_string = parseDMString(char_string, indicator);

			clearCodeIndicator(indicator);
			indicator_stack_top--;
			indicator = indicator_stack + indicator_stack_top;
			pushMeta((MemorySpace *) dm_string, indicator);
			continue;

		}

		if (indicator->parse_status == 61) { // //comments
			if (isLineEnd(localChar)) {
				clearCodeIndicator(indicator);
				indicator_stack_top--;
				indicator = indicator_stack + indicator_stack_top;
				last_index = i + 2;
			} else {
				continue;
			}
		}

		if (isLetter(localChar)) {
			if (indicator->isInLine == NOT_IN_LINE) {
				indicator->isInLine = IN_LINE;
			}
			if (indicator->isInWord == NOT_IN_WORD) {
				indicator->isInWord = IN_WORD;
			}
			continue;
		} else {
			if (indicator->isInWord == IN_WORD && indicator->isInLine == IN_LINE) {
				if (last_index < i) {
					MemorySpace * element = parseElement(char_string, last_index, i);
					pushMeta(element, indicator);
				}
				last_index = i + 1;
				indicator->isInWord = NOT_IN_WORD;
			}
		}

		if (indicator->parse_status != 74) {
			if (indicator->includer == NULL) {

			} else if (indicator->includer->type == 80) {
				DMJSON * last_includer = indicator->includer;

				indicator_stack_top++;
				indicator = indicator_stack + indicator_stack_top;
				indicator->parse_status = 74;
				indicator->includer = last_includer;
				indicator->isInLine = IN_LINE;
			} else if (indicator->includer->type == 33) {
				DMJSON * last_includer = indicator->includer;

				indicator_stack_top++;
				indicator = indicator_stack + indicator_stack_top;
				indicator->parse_status = 33;
				indicator->code = last_includer;
				indicator->isInLine = IN_LINE;

			} else {

			}
		}

		if (isLineEnd(localChar)) {
			if (indicator->isInLine == NOT_IN_LINE) {
				last_index = i + 1;
				continue;
			} else {
				indicator->isInLine = NOT_IN_LINE;
			}

			if (indicator->parse_status == 74) {
				DMJSON * last_expression = indicator->includer;

				MemorySpace * last_meta = last_expression->get(-1, NOT_DEL);
				if (last_meta == NULL) {
				} else if (last_meta->type == TYPE_CODE_OPERATOR) {
					continue;
				}

				clearCodeIndicator(indicator);
				indicator_stack_top--;
				indicator = indicator_stack + indicator_stack_top;

				MemorySpace * element = resolveExpression(last_expression);
				pushElement(element, indicator);
				indicator->includer = NULL;

			}

			if (indicator->parse_status == 33) {
				DMJSON* functionReturn = indicator->code;

				clearCodeIndicator(indicator);
				indicator_stack_top--;
				indicator = indicator_stack + indicator_stack_top;

				pushElement(functionReturn->self, indicator);
				indicator->includer = NULL;
			}

			if (indicator->parse_status == 81) {
				clearCodeIndicator(indicator);
				indicator_stack_top--;
				indicator = indicator_stack + indicator_stack_top;
			}

			last_index = i + 1;
			continue;
		}

		if (isBlank(localChar)) {
			if (indicator->isInWord == NOT_IN_WORD) {
				last_index = i + 1;
				continue;
			}

			if (indicator->parse_status == 31) {
				clearCodeIndicator(indicator);
				indicator_stack_top--;
				indicator = indicator_stack + indicator_stack_top;
				last_index = i + 1;
				continue;
			}

			if (indicator->parse_status == 32) {
				clearCodeIndicator(indicator);
				indicator_stack_top--;
				indicator = indicator_stack + indicator_stack_top;
				last_index = i + 1;
				continue;
			}

			if (indicator->parse_status == 33) {

				clearCodeIndicator(indicator);
				indicator_stack_top--;
				indicator = indicator_stack + indicator_stack_top;
				last_index = i + 1;
				continue;
			}

			if (indicator->parse_status == 22) {
				clearCodeIndicator(indicator);
				indicator_stack_top--;
				indicator = indicator_stack + indicator_stack_top;
				last_index = i + 1;
				continue;
			}

			last_index = i + 1;
			continue;
		}

		if (isSeparator(localChar)) {

			if (indicator->parse_status == 74) {
				DMJSON * last_expression = indicator->includer;

				clearCodeIndicator(indicator);
				indicator_stack_top--;
				indicator = indicator_stack + indicator_stack_top;

				MemorySpace * element = resolveExpression(last_expression);
				pushElement(element, indicator);
				indicator->includer = NULL;

			}

			if (indicator->parse_status == 81) {
				clearCodeIndicator(indicator);
				indicator_stack_top--;
				indicator = indicator_stack + indicator_stack_top;
			}

			if (indicator->parse_status == 12) {
				DMJSON * last_function_definition = indicator->includer;

				if (last_function_definition == NULL) {
				} else if (last_function_definition->type == 30) {
					pushElement(last_function_definition->self, indicator);
					indicator->includer = NULL;
				} else {
					//syntax error
				}

			}
			last_index = i + 1;
			continue;
		}
		if (localChar == ':') {

			MemorySpace * key = NULL;

			if (indicator->parse_status == 74) {
				DMJSON * last_expression = indicator->includer;

				clearCodeIndicator(indicator);
				indicator_stack_top--;
				indicator = indicator_stack + indicator_stack_top;

				MemorySpace * element = resolveExpression(last_expression);

				if (element->type == TYPE_CODE_NAME) {
					key = element;
					CodeName * key_name = (CodeName *) key;
					key_name->nameType = 0x99;
				} else if (element->type == TYPE_STRING) {
					key = element;
					key->type = TYPE_CODE_NAME;
					CodeName * key_name = (CodeName *) key;
					key_name->nameType = 0x99;
				} else {
					//syntax error
				}

				indicator->includer = NULL;

				last_index = i + 1;
			}

			if (key != NULL && (indicator->parse_status == 11 || indicator->parse_status == 21)) {

				DMKeyValue * key_value = newDMKeyValue();
				key_value->key = key;

				DMJSON * parent = indicator->code;
				indicator_stack_top++;
				indicator = indicator_stack + indicator_stack_top;
				indicator->key_value = key_value;
				indicator->parse_status = 81;

				parent->set(-1, key_value->self, NOT_REPLACE);
			}
			last_index = i + 1;
			continue;
		}

		int string_state = checkString(preChar, localChar, nextChar);
		if (0 != string_state) {
			if (indicator->parse_status != 41) {
				if (string_state == 12 || string_state == 22) {
					DMString * dm_string = newDMString(0);
					pushMeta((MemorySpace *) dm_string, indicator);
					last_index = i + 1;
					continue;
				}
				indicator_stack_top++;
				indicator = indicator_stack + indicator_stack_top;
				indicator->parse_status = 41;
				indicator->flag = string_state;
				if (string_state % 10 == 1) {
					indicator->start = i + 1;
				} else if (string_state % 10 == 3) {
					indicator->start = i + 2;
				}
				continue;
			} else {
				//logic error
			}
		}

		if (localChar == '/' && nextChar == '/') {

			indicator_stack_top++;
			indicator = indicator_stack + indicator_stack_top;
			indicator->parse_status = 61;
			last_index = i + 1;
			continue;
		}

		if (isBracket(localChar)) {
			if (localChar == '{') {
				if (indicator->includer == NULL) {
					DMJSON* includer = this->json_pool->getDMJSON();
					includer->type = 80;
					indicator->includer = includer;
				}

				DMJSON * last_includer = indicator->includer;
				indicator_stack_top++;
				indicator = indicator_stack + indicator_stack_top;
				indicator->start = i;
				if (last_includer != NULL && last_includer->type == 20) {
					indicator->parse_status = 12;
					indicator->code = last_includer;
				} else if (last_includer != NULL && last_includer->type == 30) {
					indicator->parse_status = 13;
					indicator->code = last_includer;
				} else if (last_includer != NULL && last_includer->type / 10 == 4) {
					indicator->parse_status = 14;
					indicator->code = last_includer;
				} else {
					indicator->parse_status = 11;
					DMJSON* json = this->json_pool->getDMJSON();
					json->type = 0;
					indicator->code = json;
					last_includer->set(-1, json->self, NOT_REPLACE);
				}
				last_index = i + 1;
				continue;
			}

			if (localChar == '}') {

				if (indicator->parse_status == 74) {
					DMJSON * last_expression = indicator->includer;

					clearCodeIndicator(indicator);
					indicator_stack_top--;
					indicator = indicator_stack + indicator_stack_top;

					MemorySpace * element = resolveExpression(last_expression);
					pushElement(element, indicator);
					indicator->includer = NULL;
				}

				if (indicator->parse_status == 81) {
					clearCodeIndicator(indicator);
					indicator_stack_top--;
					indicator = indicator_stack + indicator_stack_top;
					last_index = i + 1;
				}

				if (indicator->parse_status / 10 != 1) {
					//syntax error
					break;
				}
				DMJSON * code = indicator->code;

				clearCodeIndicator(indicator);
				indicator_stack_top--;
				indicator = indicator_stack + indicator_stack_top;
				if (indicator->parse_status == 11 && code->type == 0) {
				} else if (indicator->parse_status == 12 || indicator->parse_status == 13 || indicator->parse_status == 14) {
					pushElement(code->self, indicator);
					indicator->includer = NULL;
				} else if (indicator->parse_status == 74) {
					clearCodeIndicator(indicator);
					indicator_stack_top--;
					indicator = indicator_stack + indicator_stack_top;
				}
				last_index = i + 1;
				continue;
			}

			if (localChar == '[') {

				if (indicator->includer == NULL) {
					DMJSON* includer = this->json_pool->getDMJSON();
					includer->type = 80;
					indicator->includer = includer;
				}

				CodeIndicator * last_indicator = indicator;
				DMJSON * last_includer = indicator->includer;

				indicator_stack_top++;
				indicator = indicator_stack + indicator_stack_top;
				indicator->start = i;

				MemorySpace * last_meta = last_indicator->last_meta;

				if (last_meta == NULL) {
					if (last_includer != NULL) {
						last_meta = last_includer->get(-1, NOT_DEL);
					}
				}
				if (last_meta == NULL) { //21:[json]
					indicator->parse_status = 21;
					DMJSON* json = this->json_pool->getDMJSON();
					json->type = 0;
					indicator->code = json;
					last_includer->set(-1, json->self, NOT_REPLACE);
				} else if (last_meta->type == TYPE_CODE_KEYWORD) { //has with 22:[enum] in: 21:[json]
					DMString *last_keyword = (DMString *) last_meta;

					if (*last_keyword == "with" || *last_keyword == "has") {
						indicator->parse_status = 22;
						DMJSON* enumDefinition = this->json_pool->getDMJSON();
						enumDefinition->type = 61;
						indicator->code = enumDefinition;
						last_includer->set(-1, enumDefinition->self, NOT_REPLACE);
					} else if (*last_keyword == "in" || *last_keyword == "import") {
						indicator->parse_status = 21;
						DMJSON* json = this->json_pool->getDMJSON();
						json->type = 0;
						indicator->code = json;
						last_includer->set(-1, json->self, NOT_REPLACE);
					} else {
						//syntax error
						break;
					}

				} else if (last_meta->type == TYPE_CODE_NAME) { //23:[key|index|selector],
					indicator->parse_status = 23;
					DMJSON* selector = this->json_pool->getDMJSON();
					selector->type = 71;
					indicator->code = selector;

					last_includer->set(-1, selector->self, NOT_REPLACE);
				} else { //21:[json]
					indicator->parse_status = 21;
					DMJSON* json = this->json_pool->getDMJSON();
					json->type = 0;
					indicator->code = json;
					last_includer->set(-1, json->self, NOT_REPLACE);
				}
				last_index = i + 1;
				continue;
			}

			if (localChar == ']') {

				if (indicator->parse_status == 74) {
					DMJSON * last_expression = indicator->includer;

					clearCodeIndicator(indicator);
					indicator_stack_top--;
					indicator = indicator_stack + indicator_stack_top;

					MemorySpace * element = resolveExpression(last_expression);
					pushElement(element, indicator);
					indicator->includer = NULL;
				}

				if (indicator->parse_status == 81) {
					clearCodeIndicator(indicator);
					indicator_stack_top--;
					indicator = indicator_stack + indicator_stack_top;
					last_index = i + 1;
				}

				if (indicator->parse_status / 10 != 2) {
					//syntax error
					break;
				}
				DMJSON * code = indicator->code;

				CodeIndicator * last_indicator = indicator;

				indicator_stack_top--;
				indicator = indicator_stack + indicator_stack_top;

				clearCodeIndicator(last_indicator);
				if (indicator->parse_status == 21 && code->type == 0) {
				} else if (indicator->parse_status == 12 || indicator->parse_status == 13 || indicator->parse_status == 14) {
					pushElement(code->self, indicator);
					indicator->includer = NULL;
				} else if (indicator->parse_status == 74) {
					clearCodeIndicator(indicator);
					indicator_stack_top--;
					indicator = indicator_stack + indicator_stack_top;
				}

				last_index = i + 1;
				continue;
			}

			if (localChar == '(') {
				if (indicator->includer == NULL) {
					DMJSON* includer = this->json_pool->getDMJSON();
					includer->type = 80;
					indicator->includer = includer;
				}

				CodeIndicator * last_indicator = indicator;
				if (last_indicator->parse_status == 33) {
					last_index = i + 1;
					continue;
				}
				DMJSON * last_includer = indicator->includer;

				indicator_stack_top++;
				indicator = indicator_stack + indicator_stack_top;
				indicator->start = i;
				if (last_includer != NULL && last_includer->type == 30) {
					indicator->parse_status = 31;
					indicator->code = last_includer;
				} else if (last_includer != NULL && last_includer->type / 10 == 4) {
					indicator->parse_status = 35;
					indicator->code = last_includer;
//					last_includer->set(-1, last_includer->self, NOT_REPLACE);

				} else if (last_includer != NULL && last_includer->type == 33) {
					indicator_stack_top--;
					indicator = indicator_stack + indicator_stack_top;
					if (indicator->parse_status != 33) {
						//syntax error
						break;
					}
				} else {
					MemorySpace * last_meta = last_includer->get(-1, NOT_DEL);
					if (last_meta != NULL && last_meta->type == TYPE_CODE_NAME) { //function call ()
						if (last_includer->dm_list->length > 0) {
							CodeName * function_name = (CodeName *) last_includer->get(-1, DEL);
							if (function_name->nameType != 0x82 || function_name->nameType != 0x83) {
								function_name->nameType = 0x81;
							}
							DMJSON * functionCall = this->json_pool->getDMJSON();
							functionCall->type = 32;

							functionCall->name = (DMString *) function_name;

							indicator->parse_status = 32;
							indicator->code = functionCall;

							last_includer->set(-1, functionCall->self, NOT_REPLACE);
						} else {
							//syntax error
							break;
						}
					} else if (last_meta != NULL && last_meta->type == TYPE_JSON) { //Anonymous function definition
						if (last_includer->dm_list->length > 0) {
							MemorySpace * function_definition_space = last_includer->get(-1, NOT_DEL);

							DMJSON * functionDefinition = (DMJSON *) function_definition_space->pointer;
							if (functionDefinition->type != 30) {
								//syntax error
								break;
							}

							MemorySpace * name = last_includer->get(-3, NOT_DEL);
							if (name == NULL && indicator_stack_top >= 2) {
								CodeIndicator * last_last_indicator = indicator_stack + indicator_stack_top - 2;
								if (last_last_indicator->parse_status == 81 && last_last_indicator->key_value != NULL) {
									name = last_last_indicator->key_value->key;
								}
							}
							if (name == NULL) {
								functionDefinition->name = this->helper->no_name;
							} else if (name->type == TYPE_JSON) {
								DMJSON * selector = (DMJSON *) name->pointer;
								if (selector->type != 71) {
									//syntax error
								}
								DMString * function_name = (DMString *) selector->get(-1, NOT_DEL);
								if (function_name == NULL) {
									functionDefinition->name = this->helper->no_name;
								} else if (function_name->type == TYPE_CODE_NAME || function_name->type == TYPE_STRING) {
									functionDefinition->name = function_name;
								} else {
									functionDefinition->name = this->helper->no_name;
								}

							} else if (name->type == TYPE_CODE_NAME) {
								DMString * function_name = spliteFunctionName((DMString *) name);
								functionDefinition->name = function_name;

							} else {
								//syntax error
								break;
							}

							indicator->parse_status = 74;
							indicator->includer = functionDefinition;

							indicator_stack_top++;
							indicator = indicator_stack + indicator_stack_top;
							indicator->parse_status = 31;
							indicator->code = functionDefinition;

						} else {
							//syntax error
							break;
						}
					} else { //expression ()
						DMJSON * expression = this->json_pool->getDMJSON();
						expression->type = 10;

						indicator->parse_status = 34;
						indicator->includer = expression;

						last_includer->set(-1, expression->self, NOT_REPLACE);
					}
				}

				last_index = i + 1;
				continue;
			}

			if (localChar == ')') {

				if (indicator->parse_status == 74) {
					DMJSON * last_expression = indicator->includer;

					clearCodeIndicator(indicator);
					indicator_stack_top--;
					indicator = indicator_stack + indicator_stack_top;

					MemorySpace * element = resolveExpression(last_expression);
					pushElement(element, indicator);
					indicator->includer = NULL;
				}

				CodeIndicator * last_indicator = indicator;
				indicator_stack_top--;
				indicator = indicator_stack + indicator_stack_top;

				if (last_indicator->parse_status == 31) {
					last_indicator->code->block_start = last_indicator->code->dm_list->length;
				} else if (last_indicator->parse_status == 32) {
				} else if (last_indicator->parse_status == 33) {
					DMJSON* functionReturn = last_indicator->code;
					pushElement(functionReturn->self, indicator);
					indicator->includer = NULL;
				} else if (last_indicator->parse_status == 34) {
				} else if (last_indicator->parse_status == 35) {
					last_indicator->code->block_start = last_indicator->code->dm_list->length;
				} else {
					//syntax error
					break;
				}

				clearCodeIndicator(last_indicator);

				last_index = i + 1;
				continue;
			}
		}

		if (isOperator(localChar)) {

			if (indicator->parse_status != 51) {

				indicator_stack_top++;
				indicator = indicator_stack + indicator_stack_top;
				indicator->parse_status = 51;
				indicator->start = i;

				last_index = i;
				continue;
			}
		}

	}

	return root;
}
