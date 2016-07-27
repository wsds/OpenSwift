#include "stringifyCode.h"

void stringifyMeta(MemorySpace * meta, int level, DMString * code_string) {
	memset(code_string->char_string + code_string->used_length, '.', level * 2);
	code_string->used_length = code_string->used_length + level * 2;

	if (meta->type == TYPE_JSON) {
		code_string->used_length = code_string->used_length - level * 2;
		DMJSON * dm_json = (DMJSON *) meta->pointer;
		stringifyCode(dm_json, level, code_string);
	} else if (meta->type == TYPE_CODE_KEYWORD) {
		DMString * keyword = (DMString *) meta;
		(*code_string) * "K @@\n" % keyword;
	} else if (meta->type == TYPE_CODE_NAME) {
		DMString * name = (DMString *) meta;
		(*code_string) * "@@\n" % name;
	} else if (meta->type == TYPE_CODE_OPERATOR) {
		DMString * operator_str = (DMString *) meta;
		(*code_string) * "o @@\n" % operator_str;
	} else if (meta->type == TYPE_STRING) {
		DMString * str = (DMString *) meta;
		if (str->max_length == 0) {
			(*code_string) * "S @@\n" % "EMPTY";
		} else {
			(*code_string) * "S \"@@\"\n" % str;
		}
	} else if (meta->type == TYPE_NUMBER) {
		DMInt32 * number32 = (DMInt32 *) meta;
		(*code_string) * "N @@\n" % number32->number;
	} else if (meta->type == TYPE_KEY_VALUE) {
		DMKeyValue * key_value = (DMKeyValue *) meta->pointer;
		DMString * key = (DMString *) key_value->key;
		(*code_string) * "@@:\n" % key;
		if (key_value->value->type == TYPE_JSON) {
			stringifyMeta(key_value->value, level, code_string);
		} else {
			stringifyMeta(key_value->value, level + 1, code_string);
		}

	}

}

void stringifyCode(DMJSON * code_block, int level, DMString * code_string) {
	if (level > 0) {
		memset(code_string->char_string + code_string->used_length, '.', level * 2);
		code_string->used_length = code_string->used_length + level * 2;
	}

	if (code_block->type == 10) {
		(*code_string) * "E :\n";
	} else if (code_block->type == 20) {
		if (code_block->parent_name != NULL) {
			(*code_string) * "CD @@ EXTENDS @@:\n" % code_block->name % code_block->parent_name;
		} else {
			(*code_string) * "CD @@:\n" % code_block->name;
		}
	} else if (code_block->type == 30) {
		(*code_string) * "FD @@ @@:\n" % code_block->name % code_block->block_start;
	} else if (code_block->type == 32) {
		(*code_string) * "FC @@:\n" % code_block->name;
	} else if (code_block->type == 33) {
		(*code_string) * "Return :\n";
	} else if (code_block->type == 51) {
		(*code_string) * "IMPORT :\n";
	} else if (code_block->type == 61) {
		(*code_string) * "ENUM :\n";
	} else if (code_block->type == 71) {
		(*code_string) * "SELECTOR :\n";
	} else if (code_block->type == 0) {
		(*code_string) * "JSON :\n";
	} else if (code_block->type == 34) {
		(*code_string) * "ROOT :\n";
	} else if (code_block->type == 41) {
		(*code_string) * "IF @@:\n" % code_block->block_start;
	} else if (code_block->type == 45) {
		(*code_string) * "ELSEIF @@:\n" % code_block->block_start;
	} else if (code_block->type == 42) {
		(*code_string) * "FOR @@:\n" % code_block->block_start;
	} else if (code_block->type == 44) {
		(*code_string) * "WHILE @@:\n" % code_block->block_start;
	} else if (code_block->type / 10 == 4) {
		(*code_string) * "Block :\n";
	} else {
		(*code_string) * "OTHER @@ :\n" % code_block->type;
	}
	MemorySpace *sub_code;
	for (int i = 0; i < code_block->dm_list->length; i++) {
		sub_code = code_block->dm_list->get(i);
		stringifyMeta(sub_code, level + 1, code_string);
	}

}

DMString * stringifyCode(DMJSON * code_block) {
	DMString * code_string = newDMString(1024);
	stringifyCode(code_block, -1, code_string);
//	code_string->char_string[code_string->used_length] = 0;
	return code_string;

}
