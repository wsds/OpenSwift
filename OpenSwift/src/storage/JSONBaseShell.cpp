#include "JSONBaseShell.h"

JSONBaseShell * JSONBaseShell::instance = NULL;

JSONBaseShell::JSONBaseShell() {
	this->is_initialized = false;
	this->initialize();
}

JSONBaseShell * JSONBaseShell::getInstance() {
	if (instance == NULL) {
		instance = new JSONBaseShell();
	}
	return instance;
}

char * prompt_templete0 = "\033[1;36m<:> \033[1;32m";
char * prompt_templete2 = "\033[1;36m<@root_node_name@:@current_path@> \033[1;32m";


bool JSONBaseShell::initialize() {
	if (this->is_initialized == true) {
		return false;
	}
	this->is_initialized = true;

	this->input = newDMString(128);
	this->output = newDMString(128);
	this->prompt = newDMString(64);

//	char * prompt_templete = NULL;
	this->prompt->copy(prompt_templete0);

	this->root_node_name = NULL;
	this->connection_id = -1;

	this->list_root_nodes = new TraverseCallback();
	this->list_root_nodes->onGetData = &(onGetData_RootNodes);
	this->list_node_elements = new TraverseCallback();
	this->list_node_elements->onGetData = &(onGetData_NodeElements);

	return true;
}

void JSONBaseShell::shell() {
	DMString * command = newDMString(0);
	DMString * key_chain = newDMString(0);
	DMString * value_string = newDMString(0);

	int last_index = 0;
	char localChar;
	DMString * tartget_string = NULL;
	DMString * input = this->input;

	JSONBase * json_base = JSONBase::getInstance();
	bool is_new_line = true;

	DMLog(this->prompt->char_string, this->prompt->used_length);
	while (gets(input->char_string)) {
		input->used_length = getLength(input->char_string);
//		DMLog(input);
//		DMLog("abc");
		if (input->used_length > 0) {
			tartget_string = NULL;
			last_index = 0;
			for (int i = 0; i < input->used_length; i++) {
				localChar = input->char_string[i];
				if (localChar == ' ') {
					if (tartget_string == NULL) {
						tartget_string = command;
						command->char_string = input->char_string + last_index;
						command->max_length = i - last_index;

						key_chain->char_string = input->char_string + i + 1;
						key_chain->max_length = input->used_length - i - 1;
					} else if (tartget_string == command) {
						tartget_string = key_chain;
						key_chain->char_string = input->char_string + last_index;
						key_chain->max_length = i - last_index;

						value_string->char_string = input->char_string + i + 1;
						value_string->max_length = input->used_length - i - 1;
						break;
					}

					last_index = i + 1;
				}
			}
			if (tartget_string == NULL) {
				command->char_string = input->char_string;
				command->max_length = input->used_length;
			}
		}
		command->used_length = command->max_length;
		key_chain->used_length = key_chain->max_length;
		value_string->used_length = value_string->max_length;

		if (command->max_length > 0) {

			if ((*command) == "create") {

				if (key_chain->max_length == 0 || value_string->max_length == 0) {
					DMLog("create command is not complete, the correct format is as followed:\n");
					DMLog("create node_name secret");
				} else {
					DMString * node_name = key_chain;
					DMString * secret = value_string;
					RootNode * new_root_node = json_base->create(node_name, secret);

					this->root_node_space = new_root_node->self;
					this->root_node_name = new_root_node->node_name;
					this->root_node_access_key = new_root_node->access_key;

					DMLog("A json root_node is created.");
				}
			} else if ((*command) == "list" || (*command) == "ls") {

				this->output->used_length = 0;
				if (this->connection_id == -1) {
					DMLog("Following json in the database:\n");
					json_base->list_output = this->output;
					json_base->list(this->list_root_nodes);
				} else {
					JSONBaseConnection * connection = json_base->getConnection(this->connection_id, this->connection_access_key);
					MemorySpace* value = connection->list();
					DMLog("node content:\n");
					if (value == NULL) {
						DMLog("NULL");
					} else {
						if (value->type == TYPE_STRING) {
							(*this->output) * "str";
						} else if (value->type == TYPE_NUMBER) {
							(*this->output) * "num";
						} else if (value->type == TYPE_KEY_VALUE) {
							(*this->output) * "K/V";
						} else if (value->type == TYPE_JSON) {
							(*this->output) * "[";
							DMJSON * dm_json = (DMJSON *) value->pointer;
							json_base->list_output = this->output;
							dm_json->traverse(this->list_node_elements);
							(*this->output) * "]";
						}
					}
				}
				DMLog(this->output);

			} else if ((*command) == "cd:" || (*command) == "cd\\" || (*command) == "cd/") {
				//goto the root node

				if (this->connection_id == -1) {
					DMLog("connect to a json before operate.");
				} else {
					key_chain->set("root");
					JSONBaseConnection * connection = json_base->getConnection(this->connection_id, this->connection_access_key);
					bool is_done = connection->cd(key_chain);
					if (is_done == false) {
						DMLog("specified path cannot be entered!");
					} else {
						is_new_line = false;
					}
				}

			} else if ((*command) == "cd..") {
				//goto the parent node
				if (this->connection_id == -1) {
					DMLog("connect to a json before operate.");
				} else {
					key_chain->set("..");
					JSONBaseConnection * connection = json_base->getConnection(this->connection_id, this->connection_access_key);
					bool is_done = connection->cd(key_chain);
					if (is_done == false) {
						DMLog("specified path cannot be entered!");
					} else {
						is_new_line = false;
					}
				}
			} else if ((*command) == "close") {
				if (this->connection_id == -1) {
					DMLog("connect to a json before operate.");
				} else {
					key_chain->set("..");
					bool is_done = json_base->close(this->connection_id, this->connection_access_key);
					if (is_done == false) {
						DMLog("error when close json");
					} else {
						this->connection_id = -1;
						DMLog("json closed. you can open another json with the command 'open'");
					}
				}

			} else if ((*command) == "open" || (*command) == "cd") {

				if (this->connection_id == -1) {
					if (key_chain->max_length == 0) {
						DMLog("open command is not complete, the correct format is as followed:\n");
						DMLog("open node_name secret|access_key");
					} else {
						DMString * node_name = key_chain;
						DMString * access_key = value_string;
						if (value_string->max_length == 0) {
							access_key = this->root_node_access_key;
						}
						JSONBaseConnection * new_connection = json_base->open(node_name, access_key);
						if (new_connection != NULL) {
							this->connection_id = new_connection->id;
							this->connection_access_key = new_connection->access_key;
							this->current_path = new_connection->current_path;

							this->root_node_name = new_connection->root_node->node_name;
							is_new_line = false;
						} else {
							DMLog("specified json cannot be opened!");
						}
					}
				} else {
					if (key_chain->max_length == 0) {
						DMLog("cd command is not complete, the correct format is as followed:\n");
						DMLog("cd key_chain");
					} else {
						JSONBaseConnection * connection = json_base->getConnection(this->connection_id, this->connection_access_key);
						bool is_done = connection->cd(key_chain);
						if (is_done == false) {
							DMLog("specified path cannot be entered!");
						} else {
							is_new_line = false;
						}
					}
				}
			} else if ((*command) == "set" || (*command) == "put") {

				if (this->connection_id == -1) {
					DMLog("connect to a json before operate.");
				} else if (key_chain->max_length == 0 || value_string->max_length == 0) {
					DMLog("set|put command is not complete, the correct format is as followed:\n");
					DMLog("set|put key_chain value");
				} else {
					MemorySpace * value = NULL;
					if (value == NULL) {
						bool is_number = isNumber(value_string->char_string, value_string->max_length);
						if (is_number == true) {
							DMInt32 * dm_number = newDMInt32();
							dm_number->number = parseStringToNubmer(value_string->char_string, value_string->max_length);
							value = (MemorySpace *) dm_number;
						}
					}

					if (value == NULL) {
						bool is_json = isJSON(value_string->char_string, value_string->max_length);
						if (is_json == true) {
							DMJSON * dm_json = parseDMJSON(value_string->char_string, value_string->max_length);
							value = dm_json->self;
						}
					}

					if (value == NULL) {
						DMString * dm_string = newDMString(value_string->max_length);
						dm_string->copy(value_string->char_string, value_string->max_length);
						value = (MemorySpace *) dm_string;
					}

					JSONBaseConnection * connection = json_base->getConnection(this->connection_id, this->connection_access_key);
					bool is_done = false;
					if ((*command) == "set") {
						is_done = connection->set(key_chain, value);
					} else {
						is_done = connection->put(key_chain, value);
					}

					if (is_done == false) {
						DMLog("error when set|put!");
					} else {
						DMLog("done!");
					}

				}
			} else if ((*command) == "get") {
				if (this->connection_id == -1) {
					DMLog("connect to a json before operate.");
				} else {
					if (key_chain->max_length == 0) {
						key_chain->set("this");
					}

					JSONBaseConnection * connection = json_base->getConnection(this->connection_id, this->connection_access_key);
					MemorySpace* value = connection->get(key_chain);

					if (value == NULL) {
						DMLog("NULL");
					} else {
						DMLog(value);
					}
				}

			} else if ((*command) == "del") {

				if (this->connection_id == -1) {
					DMLog("connect to a json before operate.");
				} else if (key_chain->max_length == 0) {
					DMLog("del command is not complete, the correct format is as followed:\n");
					DMLog("del key_chain");
				} else {

					JSONBaseConnection * connection = json_base->getConnection(this->connection_id, this->connection_access_key);
					MemorySpace* value = connection->del(key_chain);

					if (value == NULL) {
						DMLog("NULL");
					} else {
						DMLog(value);
					}

				}
			} else if ((*command) == "quit") {
				break;
			} else {
				DMLog("Unknown command?");
			}

		} else {
			is_new_line = false;
		}

		if (this->connection_id == -1) {
			this->prompt->copy(prompt_templete0);
		} else {
			this->prompt->copy(prompt_templete2);
			this->prompt->format(this->root_node_name);
			this->prompt->format(this->current_path);
		}
		command->max_length = 0;
		key_chain->max_length = 0;
		value_string->max_length = 0;

		if (is_new_line == true) {
			DMLog("\n");
		}
		is_new_line = true;

		DMLog(this->prompt->char_string, this->prompt->used_length);
		clearCurrentColor();

	}
}
