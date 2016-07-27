#include "DMJSON.h"

DMJSON * newDMJSON() {
	MemorySpace * space = swift_malloc(sizeof(DMJSON));
	space->type = TYPE_JSON;

	DMJSON * dm_json = (DMJSON *) space->pointer;
	dm_json->self = space;
	dm_json->initialize();
	return dm_json;
}


//todo for small json the memory usage should be grouped together.
//todo for big json the big container of the memory usage should be split out

bool DMJSON::initialize() {
	if (this->is_initialized == 0x80031008) {
		return true;
	}
	this->length = 0;

	this->dm_list = newDMLIST();
	this->dm_list_space = this->dm_list->self;
	this->dm_hashTable = newDMHashTable();
	this->dm_hashTable_space = this->dm_hashTable->self;

	this->is_initialized = 0x80031008;

	this->temp_key = newDMString(0);

	return true;
}

void DMJSON::free() {
	this->dm_list->free();
	this->dm_hashTable->free();

	swift_free(this->self);
}

void DMJSON::set(int index, MemorySpace* value, bool isreplace) {
	this->dm_list->set(index, value, isreplace);
	if (index == -1 && value->type == TYPE_KEY_VALUE && isreplace == NOT_REPLACE) {
		DMKeyValue* dm_key_value = (DMKeyValue*) value->pointer;
		this->dm_hashTable->push(dm_key_value);
	}
}

MemorySpace* DMJSON::get(int index, bool isdel) {
	MemorySpace* value = this->dm_list->get(index, isdel);
	if (value != NULL && value->type == TYPE_KEY_VALUE && isdel == DEL) {
		DMKeyValue* dm_key_value = (DMKeyValue*) value->pointer;
		this->dm_hashTable->del(dm_key_value->key);
	}
	return value;
}

//hash
void DMJSON::set(MemorySpace* key, MemorySpace* value) {

	DMKeyValue * dm_key_value = this->dm_hashTable->set(key, value);
	if (dm_key_value->is_replaced == 0xf1) {
		dm_key_value->is_replaced = 0x00;
	} else if (dm_key_value->is_replaced == 0x00) {
		this->dm_list->set(-1, dm_key_value->self);
	}
}
MemorySpace* DMJSON::get(MemorySpace* key) {
	MemorySpace* value = this->dm_hashTable->get(key);
	return value;
}

DMKeyValue* DMJSON::getKV(MemorySpace* key) {
	DMKeyValue* value = this->dm_hashTable->getKV(key);
	return value;
}

MemorySpace* DMJSON::del(MemorySpace* key) {
	DMKeyValue * key_value = this->dm_hashTable->del(key);
	int index = this->dm_list->findValue(key_value->self);
	this->dm_list->del(index);
	return key_value->value;
}

void DMJSON::push(MemorySpace * dm_key_value) {
	this->set(-1, dm_key_value, NOT_REPLACE);
}
void DMJSON::traverse(TraverseCallback * traverseCallback) {
//	this->dm_list = (DMLIST *) this->dm_list_space->pointer;
	this->dm_list->traverse(traverseCallback);
}
//******************************JSONBASE*************************************

bool DMJSON::put(DMString * key_chain, MemorySpace * value) {

	int last_index = 0;
	char localChar;
	DMJSON * target_json = this;
	MemorySpace * target_space;
	DMString * target_key = this->temp_key;

	int target_index;
	bool as_list = false;

	for (int i = 0; i < key_chain->max_length; i++) {
		localChar = key_chain->char_string[i];

		if (localChar == '.' || localChar == '#') {

			target_key->char_string = key_chain->char_string + last_index;
			target_key->max_length = i - last_index;
			if (target_key->max_length > 0) {

				if (as_list == true) {
					target_index = parseStringToNubmer(target_key->char_string, target_key->max_length);
					as_list = false;
					target_space = target_json->get(target_index, NOT_DEL);
					if (target_space == NULL) {
						return false;
					}

					if (target_space->type == TYPE_KEY_VALUE) {
						DMKeyValue* key_value = (DMKeyValue *) target_space->pointer;
						target_space = key_value->value;
					}

					if (target_space->type != TYPE_JSON) {
						DMJSON * new_json = newDMJSON();
						new_json->set(-1, new_json->self, NOT_REPLACE);
						exchangeSpace(target_space, new_json->self);
					}

					target_json = (DMJSON *) target_space->pointer;

				} else {
					if ((*target_key) == "this") {
						target_space = target_json->self;
					} else {
						target_space = target_json->get((MemorySpace *) target_key);
						if (target_space != NULL) {
							if (target_space->type != TYPE_JSON) {
								DMJSON * new_json = newDMJSON();
								new_json->set(-1, new_json->self, NOT_REPLACE);
								exchangeSpace(target_space, new_json->self);
							}
							target_json = (DMJSON *) target_space->pointer;
						} else {
							DMJSON * new_json = newDMJSON();
							DMString * new_key = newDMString(target_key->max_length);
							new_key->copy(target_key->char_string, target_key->max_length);

							target_json->set((MemorySpace *) new_key, new_json->self);
							target_json = new_json;
						}
					}
				}
			}
			last_index = i + 1;
		}

		if (localChar == '#') {
			as_list = true;
		}

		if (i == key_chain->max_length - 1) {
			target_key->char_string = key_chain->char_string + last_index;
			target_key->max_length = i + 1 - last_index;
			if (!target_key->max_length > 0) {
				return false;
			}

			if (as_list == true) {
				target_index = parseStringToNubmer(target_key->char_string, target_key->max_length);
				target_json->set(target_index, value, NOT_REPLACE);

			} else {
				target_space = target_json->get((MemorySpace *) target_key);
				if (target_space != NULL) {

					if (target_space->type == TYPE_JSON) {
						target_json = (DMJSON *) target_space->pointer;
						target_json->set(-1, value, NOT_REPLACE);
					} else if (target_space->type == TYPE_STRING || target_space->type == TYPE_NUMBER) {
						DMJSON * new_json = newDMJSON();
						new_json->set(-1, new_json->self, NOT_REPLACE);
						exchangeSpace(target_space, new_json->self);
					}
				} else {
					DMString * new_key = newDMString(target_key->max_length);
					new_key->copy(target_key->char_string, target_key->max_length);
					target_key = new_key;
					target_json->set((MemorySpace *) target_key, value);
				}
			}
		}
	}

	return true;
}

bool DMJSON::set(DMString * key_chain, MemorySpace * value) {

	int last_index = 0;
	char localChar;
	DMJSON * target_json = this;
	MemorySpace * target_space;
	DMString * target_key = this->temp_key;

	int target_index;
	bool as_list = false;

	for (int i = 0; i < key_chain->max_length; i++) {
		localChar = key_chain->char_string[i];

		if (localChar == '.' || localChar == '#') {

			target_key->char_string = key_chain->char_string + last_index;
			target_key->max_length = i - last_index;
			if (target_key->max_length > 0) {

				if (as_list == true) {
					target_index = parseStringToNubmer(target_key->char_string, target_key->max_length);
					as_list = false;
					target_space = target_json->get(target_index, NOT_DEL);
					if (target_space == NULL) {
						return false;
					}

					if (target_space->type == TYPE_KEY_VALUE) {
						DMKeyValue* key_value = (DMKeyValue *) target_space->pointer;
						target_space = key_value->value;
					}

					if (target_space->type != TYPE_JSON) {
						DMJSON * new_json = newDMJSON();
						new_json->set(-1, new_json->self, NOT_REPLACE);
						exchangeSpace(target_space, new_json->self);
					}

					target_json = (DMJSON *) target_space->pointer;

				} else {
					if ((*target_key) == "this") {
						target_space = target_json->self;
					} else {
						target_space = target_json->get((MemorySpace *) target_key);
						if (target_space != NULL) {
							if (target_space->type != TYPE_JSON) {
								DMJSON * new_json = newDMJSON();
								new_json->set(-1, new_json->self, NOT_REPLACE);
								exchangeSpace(target_space, new_json->self);
							}
							target_json = (DMJSON *) target_space->pointer;
						} else {
							DMJSON * new_json = newDMJSON();
							DMString * new_key = newDMString(target_key->max_length);
							new_key->copy(target_key->char_string, target_key->max_length);

							target_json->set((MemorySpace *) new_key, new_json->self);
							target_json = new_json;
						}
					}
				}
			}
			last_index = i + 1;
		}

		if (localChar == '#') {
			as_list = true;
		}

		if (i == key_chain->max_length - 1) {
			target_key->char_string = key_chain->char_string + last_index;
			target_key->max_length = i + 1 - last_index;
			if (!target_key->max_length > 0) {
				return false;
			}

			if (as_list == true) {
				target_index = parseStringToNubmer(target_key->char_string, target_key->max_length);
				target_json->set(target_index, value, REPLACE);

			} else {
				MemorySpace *old_value = target_json->get((MemorySpace *) target_key);
				if (old_value != NULL) {
					exchangeSpace(old_value, value);
				} else {
					DMString * new_key = newDMString(target_key->max_length);
					new_key->copy(target_key->char_string, target_key->max_length);
					target_key = new_key;
					target_json->set((MemorySpace *) target_key, value);
				}
			}
		}
	}

	return true;
}

DMKeyValue * DMJSON::getKV(DMString * key_chain) {
    if (key_chain->used_length == 0) {
        return NULL;
    }
    DMKeyValue * dm_key_value = NULL;

    int last_index = 0;
    char localChar;
    DMJSON * target_json = this;
    MemorySpace * target_space;
    DMString * target_key = this->temp_key;
    int target_index;
    bool as_list = false;

    for (int i = 0; i < key_chain->used_length; i++) {
        localChar = key_chain->char_string[i];

        if (localChar == '.' || localChar == '#') {

            target_key->char_string = key_chain->char_string + last_index;
            target_key->max_length = i - last_index;
            if (target_key->max_length > 0) {
                if (as_list == true) {
                    target_index = parseStringToNubmer(target_key->char_string, target_key->max_length);
                    as_list = false;
                    target_space = target_json->get(target_index, NOT_DEL);
                    if (target_space == NULL) {
                        return NULL;
                    }

                    if (target_space->type == TYPE_JSON) {
                        target_json = (DMJSON *) target_space->pointer;
                    } else if (target_space->type == TYPE_STRING || target_space->type == TYPE_NUMBER) {
                        return NULL;
                    } else if (target_space->type == TYPE_KEY_VALUE) {
                        DMKeyValue* key_value = (DMKeyValue *) target_space->pointer;
                        target_space = key_value->value;
                        if (target_space->type == TYPE_JSON) {
                            target_json = (DMJSON *) target_space->pointer;
                        } else {
                            return NULL;
                        }
                    }

                } else {
                    if ((*target_key) == "this") {
                        target_space = target_json->self;
                    } else {
                        target_space = target_json->get((MemorySpace *) target_key);
                        if (target_space != NULL) {
                            if (target_space->type == TYPE_JSON) {
                                target_json = (DMJSON *) target_space->pointer;
                            } else if (target_space->type == TYPE_STRING || target_space->type == TYPE_NUMBER) {
                                DMJSON * new_json = newDMJSON();
                                target_json->set((MemorySpace *) target_key, new_json->self);
                                new_json->set(-1, target_space, NOT_REPLACE);
                                target_json = new_json;
                            }
                        } else {
                            return NULL;
                        }
                    }
                }
            }
            last_index = i + 1;
        }

        if (localChar == '#') {
            as_list = true;
        }

        if (i == key_chain->used_length - 1) {
            target_key->char_string = key_chain->char_string + last_index;
            target_key->max_length = i + 1 - last_index;
            if (!target_key->max_length > 0) {
                return NULL;
            }

            if (as_list == true) {
//                target_index = parseStringToNubmer(target_key->char_string, target_key->max_length);
//                dm_key_value = target_json->get(target_index, NOT_DEL);
            } else {
                dm_key_value = target_json->getKV((MemorySpace *) target_key);
            }
        }
    }

    return dm_key_value;
}

MemorySpace * DMJSON::get(DMString * key_chain) {
	if (key_chain->used_length == 0) {
		return this->self;
	}
	MemorySpace * value = NULL;

	int last_index = 0;
	char localChar;
	DMJSON * target_json = this;
	MemorySpace * target_space;
	DMString * target_key = this->temp_key;
	int target_index;
	bool as_list = false;

	for (int i = 0; i < key_chain->used_length; i++) {
		localChar = key_chain->char_string[i];

		if (localChar == '.' || localChar == '#') {

			target_key->char_string = key_chain->char_string + last_index;
			target_key->max_length = i - last_index;
			if (target_key->max_length > 0) {
				if (as_list == true) {
					target_index = parseStringToNubmer(target_key->char_string, target_key->max_length);
					as_list = false;
					target_space = target_json->get(target_index, NOT_DEL);
					if (target_space == NULL) {
						return NULL;
					}

					if (target_space->type == TYPE_JSON) {
						target_json = (DMJSON *) target_space->pointer;
					} else if (target_space->type == TYPE_STRING || target_space->type == TYPE_NUMBER) {
						return NULL;
					} else if (target_space->type == TYPE_KEY_VALUE) {
						DMKeyValue* key_value = (DMKeyValue *) target_space->pointer;
						target_space = key_value->value;
						if (target_space->type == TYPE_JSON) {
							target_json = (DMJSON *) target_space->pointer;
						} else {
							return NULL;
						}
					}

				} else {
					if ((*target_key) == "this") {
						target_space = target_json->self;
					} else {
						target_space = target_json->get((MemorySpace *) target_key);
						if (target_space != NULL) {
							if (target_space->type == TYPE_JSON) {
								target_json = (DMJSON *) target_space->pointer;
							} else if (target_space->type == TYPE_STRING || target_space->type == TYPE_NUMBER) {
								DMJSON * new_json = newDMJSON();
								target_json->set((MemorySpace *) target_key, new_json->self);
								new_json->set(-1, target_space, NOT_REPLACE);
								target_json = new_json;
							}
						} else {
							return NULL;
						}
					}
				}
			}
			last_index = i + 1;
		}

		if (localChar == '#') {
			as_list = true;
		}

		if (i == key_chain->used_length - 1) {
			target_key->char_string = key_chain->char_string + last_index;
			target_key->max_length = i + 1 - last_index;
			if (!target_key->max_length > 0) {
				return NULL;
			}

			if (as_list == true) {
				target_index = parseStringToNubmer(target_key->char_string, target_key->max_length);
				value = target_json->get(target_index, NOT_DEL);
			} else {
				value = target_json->get((MemorySpace *) target_key);

//				if (value->type == TYPE_FUNCTION) {//it can be determined by the runtime;
//					DefinedFunctionMirror * dm_function = (DefinedFunctionMirror *) value->pointer;
//					dm_function->parent = target_json;
//				}
			}
		}
	}

	return value;
}

MemorySpace * DMJSON::del(DMString * key_chain) {
	MemorySpace * value = NULL;

	int last_index = 0;
	char localChar;
	DMJSON * target_json = this;
	MemorySpace * target_space;
	DMString * target_key = this->temp_key;
	int target_index;
	bool as_list = false;

	for (int i = 0; i < key_chain->used_length; i++) {
		localChar = key_chain->char_string[i];

		if (localChar == '.' || localChar == '#') {

			target_key->char_string = key_chain->char_string + last_index;
			target_key->max_length = i - last_index;
			if (target_key->max_length > 0) {
				if (as_list == true) {
					target_index = parseStringToNubmer(target_key->char_string, target_key->max_length);
					as_list = false;
					target_space = target_json->get(target_index, NOT_DEL);
					if (target_space == NULL) {
						return NULL;
					}

					if (target_space->type == TYPE_JSON) {
						target_json = (DMJSON *) target_space->pointer;
					} else if (target_space->type == TYPE_STRING || target_space->type == TYPE_NUMBER) {
						return NULL;
					} else if (target_space->type == TYPE_KEY_VALUE) {
						DMKeyValue* key_value = (DMKeyValue *) target_space->pointer;
						target_space = key_value->value;
						if (target_space->type == TYPE_JSON) {
							target_json = (DMJSON *) target_space->pointer;
						} else {
							return NULL;
						}
					}

				} else {
					if ((*target_key) == "this") {
						target_space = target_json->self;
					} else {
						target_space = target_json->get((MemorySpace *) target_key);
						if (target_space != NULL) {
							if (target_space->type == TYPE_JSON) {
								target_json = (DMJSON *) target_space->pointer;
							} else if (target_space->type == TYPE_STRING || target_space->type == TYPE_NUMBER) {
								DMJSON * new_json = newDMJSON();
								target_json->set((MemorySpace *) target_key, new_json->self);
								new_json->set(-1, target_space, NOT_REPLACE);
								target_json = new_json;
							}
						} else {
							return NULL;
						}
					}
				}
			}

			last_index = i + 1;

		}

		if (localChar == '#') {
			as_list = true;
		}

		if (i == key_chain->used_length - 1) {
			target_key->char_string = key_chain->char_string + last_index;
			target_key->max_length = i + 1 - last_index;
			if (!target_key->max_length > 0) {
				return NULL;
			}

			if (as_list == true) {
				target_index = parseStringToNubmer(target_key->char_string, target_key->max_length);
				value = target_json->get(target_index, DEL);
			} else {
				value = target_json->del((MemorySpace *) target_key);
			}
		}
	}

	return value;

}

//******************************JSONBASE*************************************

void rePointSelf(MemorySpace * object) {
	if (object->type == TYPE_JSON) {
		DMJSON * dm_json = (DMJSON*) object->pointer;
		dm_json->self = object;
	} else if (object->type == TYPE_KEY_VALUE) {
		DMKeyValue * dm_key_value = (DMKeyValue *) object->pointer;
		dm_key_value->self = object;
	}
}

bool exchangeSpace(MemorySpace * left, MemorySpace * right) {
	map_exchange(left, right);
	rePointSelf(left);
	rePointSelf(right);
	return true;
}
