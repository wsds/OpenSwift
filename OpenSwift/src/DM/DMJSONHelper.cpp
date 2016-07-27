#include "DMJSONHelper.h"

MemorySpace* parseObject(char* string, DMJSONIndicator* object_indicator) {
	MemorySpace* object;
	int start = object_indicator->head;
	int end = object_indicator->tail;
	char localChar;
	while (true) {
		localChar = string[start];
		if (localChar == TAB || localChar == COMMA || localChar == ENTER || localChar == BR || localChar == BLANK) {
			start++;
		} else {
			break;
		}
	}

	while (true) {
		localChar = string[end - 1];
		if (localChar == TAB || localChar == COMMA || localChar == ENTER || localChar == BR || localChar == BLANK) {
			end--;
		} else {
			break;
		}
	}
	if (start >= end) {
		return NULL;
	}

	int length = end - start - object_indicator->quotes_count * 2;
	char *tartget_string = string + start + object_indicator->quotes_count;
	bool is_number = isNumber(tartget_string, length);
	if (is_number == false || object_indicator->quotes_count != 0) {

		DMString * dm_string = newDMString(0);
		dm_string->char_string = tartget_string;
		dm_string->used_length = length;
		dm_string->max_length = length;
		object = (MemorySpace *) dm_string;
	} else {
		DMInt32 * dm_number = newDMInt32();
		dm_number->number = parseStringToNubmer(tartget_string, length);
		object = (MemorySpace *) dm_number;
	}

	return object;
}

#define json_indicator_stack_size  10
#define key_value_stack_size  10

int json_indicator_stack_top = 0;
int key_value_stack_top = 0;

DMJSONIndicator json_indicator_stack[json_indicator_stack_size];
DMKeyValue* key_value_indicator_stack[key_value_stack_size];

DMJSON* parseDMJSON(const char* char_string) {
	int string_length = strlen(char_string);
	return parseDMJSON((char*) char_string, string_length);
}
DMJSON* parseDMJSON(char* char_string, int string_length) {

	char localChar;

	DMString * json_string = newDMString(string_length);
	json_string->copy(char_string, string_length);
	char* json_char_string = json_string->char_string;

	int parsingStatus = 1; //[1:normal,2:quote_start,3:string,4:second_quote_start,5:quote_stop,6:second_quote_stop],[1-2,2-3,3-1,2-4,4-5,5-3,4-1]
	int QuoteStatus = 11; //[ 11 = ' , 13 = ''' , 21 = " , 23 = """]

	int last_COMMA_index = 0;

	DMJSONIndicator* json_indicator = NULL;
	DMJSONIndicator* object_indicator = NULL;
	DMJSON* result_json = NULL;

	json_indicator_stack_top = 0;
	key_value_stack_top = 0;

	for (int i = 0; i < string_length; i++) {
		localChar = json_char_string[i];

		if (parsingStatus == 3) {

			if ((localChar == SINGLEQUOTE && QuoteStatus == 11) || (localChar == DOUBLEQUOTES && QuoteStatus == 21)) {
				json_indicator->quotes_count = 1;
				parsingStatus = 1;
				continue;
			} else if ((localChar == SINGLEQUOTE && QuoteStatus == 13) || (localChar == DOUBLEQUOTES && QuoteStatus == 23)) {
				parsingStatus = 5;
			}
		} else if (parsingStatus == 2) {
			if ((localChar == SINGLEQUOTE && QuoteStatus == 11) || (localChar == DOUBLEQUOTES && QuoteStatus == 21)) {
				parsingStatus = 4;
			} else {
				parsingStatus = 3;
			}
		} else if (parsingStatus == 4) {
			if (localChar == SINGLEQUOTE && QuoteStatus == 11) {
				parsingStatus = 3;
				QuoteStatus = 13;
			} else if (localChar == DOUBLEQUOTES && QuoteStatus == 21) {
				parsingStatus = 3;
				QuoteStatus = 23;
			} else {
				json_indicator->quotes_count = 1;
				parsingStatus = 1;
			}
		} else if (parsingStatus == 5) {
			if ((localChar == SINGLEQUOTE && QuoteStatus == 13) || (localChar == DOUBLEQUOTES && QuoteStatus == 23)) {
				parsingStatus = 6;
			} else {
				parsingStatus = 3;
			}
		} else if (parsingStatus == 6) {
			if ((localChar == SINGLEQUOTE && QuoteStatus == 13) || (localChar == DOUBLEQUOTES && QuoteStatus == 23)) {
				json_indicator->quotes_count = 3;
				parsingStatus = 1;
				continue;
			} else {
				parsingStatus = 3;
			}
		}

		//*************************************parsingStatus == 1*****************************************************************************************************
		if (parsingStatus == 1) {
			if (localChar == COMMA) {
				if (i > last_COMMA_index) {
					object_indicator = json_indicator_stack + json_indicator_stack_top;
					object_indicator->head = last_COMMA_index;
					object_indicator->tail = i;
					object_indicator->quotes_count = json_indicator->quotes_count;
					json_indicator->quotes_count = 0;

					last_COMMA_index = i + 1;

					MemorySpace* object = parseObject(json_char_string, object_indicator);
					if (object == NULL) {
						continue;
					}
					if (key_value_stack_top > 0 && key_value_indicator_stack[key_value_stack_top - 1]->value == NULL) {
						DMKeyValue * dm_key_value = key_value_indicator_stack[key_value_stack_top - 1];
						dm_key_value->value = object;
						key_value_stack_top--;
					} else {
						json_indicator->dm_json->push(object);
					}

				}
			} else if (localChar == COLON) {
				if (i > last_COMMA_index) {
					object_indicator = json_indicator_stack + json_indicator_stack_top;
					object_indicator->head = last_COMMA_index;
					object_indicator->tail = i;
					object_indicator->quotes_count = json_indicator->quotes_count;
					json_indicator->quotes_count = 0;

					last_COMMA_index = i + 1;

					MemorySpace * dm_key = parseObject(json_char_string, object_indicator);
					if (dm_key == NULL) {
						continue;
					}
					if (key_value_stack_top < key_value_stack_size) {
						DMKeyValue * dm_key_value = newDMKeyValue();
						dm_key_value->key = dm_key;
						key_value_indicator_stack[key_value_stack_top] = dm_key_value;
						key_value_stack_top++;

						json_indicator->dm_json->push(dm_key_value->self);
					} else {
						//report error;
					}
				}
			} else if (localChar == SINGLEQUOTE) {
				QuoteStatus = 11;
				parsingStatus = 2;
			} else if (localChar == DOUBLEQUOTES) {
				QuoteStatus = 21;
				parsingStatus = 2;
			} else if (localChar == LEFTBRACKET || localChar == LEFTBIGBRACKET) {
				if (json_indicator_stack_top < json_indicator_stack_size) {

					DMJSON* json = newDMJSON();

					DMJSONIndicator* new_json_indicator = json_indicator_stack + json_indicator_stack_top;
					json_indicator_stack_top++;
					new_json_indicator->head = i;
					new_json_indicator->dm_json = json;
					new_json_indicator->quotes_count = 0;
//					json->json_indicator = (void *) new_json_indicator;

					if (json_indicator_stack_top == 1) {
						result_json = json;
					} else {
						if (key_value_stack_top > 0 && key_value_indicator_stack[key_value_stack_top - 1]->value == NULL) {
							DMKeyValue * dm_key_value = key_value_indicator_stack[key_value_stack_top - 1];
							dm_key_value->value = json->self;
							key_value_stack_top--;

						} else {
							json_indicator->dm_json->push(json->self);
						}
					}

					json_indicator = new_json_indicator;

					last_COMMA_index = i + 1;
				} else {
					//report error
				}
			} else if (localChar == RIGHTBRACKET || localChar == RIGHTBIGBRACKET) {

				DMJSONIndicator* object_indicator = json_indicator_stack + json_indicator_stack_top;
				object_indicator->head = last_COMMA_index;
				object_indicator->tail = i;
				object_indicator->quotes_count = json_indicator->quotes_count;
				json_indicator->quotes_count = 0;

				MemorySpace* object = parseObject(json_char_string, object_indicator);
				if (object != NULL) {
					if (key_value_stack_top > 0 && key_value_indicator_stack[key_value_stack_top - 1]->value == NULL) {
						DMKeyValue * dm_key_value = key_value_indicator_stack[key_value_stack_top - 1];
						dm_key_value->value = object;
						key_value_stack_top--;

					} else {
						json_indicator->dm_json->push(object);
					}
				}

				last_COMMA_index = i + 1;

				if (json_indicator_stack_top > 1) {
					//resolve the last element spited by COMMA
					json_indicator = json_indicator_stack + json_indicator_stack_top - 1;
					json_indicator->tail = i;

					json_indicator = json_indicator_stack + json_indicator_stack_top - 2;
					//json_indicator->json = parseJSON(string, json_indicator);
					json_indicator_stack_top--;
				} else if (json_indicator_stack_top == 1) {
					if (i + 1 == string_length) {
						//resolve the last element spited by COMMA
						return result_json;
					} else {
						//report error
					}
				} else {
					//report error
				}
			}
		}
		//*************************************parsingStatus == 1*****************************************************************************************************

	}

	return NULL;
}

int preStringifyDMObject(MemorySpace* object) {
	int offset = 0;

	if (object == NULL) {
		offset = offset + 4;
	} else if (object->type == TYPE_NUMBER) {
		DMInt32 * dm_int = (DMInt32*) object;
		offset = numberToString(dm_int->number, NULL);
	} else if (object->type == TYPE_STRING) {
		DMString * dm_string = (DMString*) object;
		if (dm_string->max_length == 1) {
			offset = offset + 3;
		} else {
			bool hasQuote = false;
			for (int j = 0; j < dm_string->max_length; j++) {
				if (dm_string->char_string[j] == SINGLEQUOTE || dm_string->char_string[j] == DOUBLEQUOTES) {
					hasQuote = true;
					break;
				}
			}
			offset = offset + dm_string->max_length;
			if (hasQuote) {
				offset = offset + 6;
			} else {
				offset = offset + 2;
			}
		}

	} else if (object->type == TYPE_FUNCTION) {
		DefinedFunctionMirror * dm_function = (DefinedFunctionMirror *) object->pointer;
		offset = offset + 12 + dm_function->name->used_length;
	} else if (object->type == TYPE_KEY_VALUE) {
		DMKeyValue * key_value = (DMKeyValue*) object->pointer;

		if (HasKeyQuote == true) {
			offset = 3 + key_value->key->length;
		} else {
			offset = 1 + key_value->key->length;
		}
		offset = offset + preStringifyDMObject(key_value->value);
	} else if (object->type == TYPE_JSON) {
		DMJSON* json = (DMJSON*) object->pointer;

		int sub_offset = 0;
		MemorySpace *sub_object;
		for (int i = 0; i < json->dm_list->length; i++) {
			if (i != 0) {
				sub_offset++;
			}
			sub_object = json->dm_list->get(i);
			sub_offset = sub_offset + preStringifyDMObject(sub_object);
		}
		offset = sub_offset + 2;
	}
	return offset;
}

int stringifyDMObject(MemorySpace* object, DMString* json_string, DMJSON* parent) {

	if (object == NULL) {
		(*json_string) * nullStr;
	} else if (object->type == TYPE_NUMBER) {
		DMInt32 * dm_int = (DMInt32*) object;
		(*json_string) + dm_int->number;
	} else if (object->type == TYPE_STRING) {
		DMString * dm_string = (DMString*) object;
		bool hasQuote = false;
		for (int j = 0; j < dm_string->max_length; j++) {
			if (dm_string->char_string[j] == SINGLEQUOTE || dm_string->char_string[j] == DOUBLEQUOTES) {
				hasQuote = true;
				break;
			}
		}

		if (hasQuote) {
//			js_string->char_string = json_string->char_string + json_string->length + 3;
			(*json_string) * ("\"\"\"@@\"\"\"") % (dm_string);
		} else {
//			js_string->char_string = json_string->char_string + json_string->length + 1;
			(*json_string) * ("\"@@\"") % (dm_string);
		}

	} else if (object->type == TYPE_FUNCTION) {
		DefinedFunctionMirror * dm_function = (DefinedFunctionMirror *) object->pointer;
		(*json_string) * ("@@(FUNCTION)") % ((dm_function->name));
	} else if (object->type == TYPE_KEY_VALUE) {
		DMKeyValue * key_value = (DMKeyValue*) object->pointer;

		if (HasKeyQuote == true) {
//			key_value->key = json_string->char_string + json_string->length + 1;
			(*json_string) * ("\"@@\":") % ((DMString*) (key_value->key));
		} else {
//			key_value->key = json_string->char_string + json_string->length;
			(*json_string) * ("@@:") % ((DMString*) (key_value->key));
		}

//		parent->hashTable->resetKey(key_value->key, key_value->keyLength);

		//parse value
		stringifyDMObject(key_value->value, json_string, NULL);
	} else if (object->type == TYPE_JSON) {
		DMJSON* json = (DMJSON*) object->pointer;

		if (useBIGBRACKET == true) {
			json_string->char_string[json_string->used_length] = LEFTBIGBRACKET;
		} else {
			json_string->char_string[json_string->used_length] = LEFTBRACKET;
		}
		json_string->used_length++;

		MemorySpace *sub_object;
		for (int i = 0; i < json->dm_list->length; i++) {
			if (i != 0) {
				json_string->char_string[json_string->used_length] = COMMA;
				json_string->used_length++;
			}
			sub_object = json->dm_list->get(i);
			stringifyDMObject(sub_object, json_string, json);
		}

		if (useBIGBRACKET == true) {
			json_string->char_string[json_string->used_length] = RIGHTBIGBRACKET;
		} else {
			json_string->char_string[json_string->used_length] = RIGHTBRACKET;
		}
		json_string->used_length++;
	}

	return 0;
}

DMString* stringifyJSON2DMString(DMJSON* json) {

	int json_string_length = preStringifyDMObject(json->self);
	DMString * json_string = newDMString(json_string_length);

	int offset = stringifyDMObject(json->self, json_string, NULL);

	return json_string;
}

void formatString(DMString * dm_string, MemorySpace * object) {
	if (object == NULL) {
		(*dm_string) % "NULL";
	} else if (object->type == TYPE_NUMBER) {
		DMInt32 * dm_int = (DMInt32 *) object;
		int targetLength = numberToString(dm_int->number, number_string_buffer);
		dm_string->format(number_string_buffer, targetLength);
	} else if (object->type == TYPE_STRING) {
		DMString * dm_object_string = (DMString *) object;
		dm_string->format(dm_object_string->char_string, dm_object_string->used_length);
	} else if (object->type == TYPE_JSON) {
		DMJSON * dm_json = (DMJSON*) object->pointer;
		DMString * dm_json_string = stringifyJSON2DMString(dm_json);
		dm_string->format(dm_json_string->char_string, dm_json_string->used_length);
	} else if (object->type == TYPE_KEY_VALUE) {
		(*dm_string) % "@@:@@";
		DMKeyValue * dm_key_value = (DMKeyValue *) object->pointer;
		formatString(dm_string, dm_key_value->key);
		formatString(dm_string, dm_key_value->value);
	} else {
		(*dm_string) % "UNKown";
	}
}
