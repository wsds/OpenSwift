#include "JSONBase.h"

RootNode * newRootNode() {
	MemorySpace * space = map_malloc(sizeof(RootNode));
	space->type = TYPE_ROOT_NODE;

	RootNode * root_node = (RootNode *) space->pointer;
	root_node->self = space;
	return root_node;
}
JSONBaseConnectionPool * newJSONBaseConnectionPool() {
	MemorySpace * space = map_malloc(sizeof(JSONBaseConnectionPool));
	space->type = TYPE_JSONBASE_CONNECTION_POOL;
	JSONBaseConnectionPool * connection_pool = (JSONBaseConnectionPool *) space->pointer;
	connection_pool->self = space;
	connection_pool->connection_pool_size = 64;
	connection_pool->current_connection_id = 0;
	connection_pool->connection_pool_start_space = map_malloc(sizeof(JSONBaseConnection) * connection_pool->connection_pool_size);
	return connection_pool;
}
JSONBase * JSONBase::instance = NULL;

JSONBase::JSONBase() {
	this->is_initialized = false;
	this->initialize();
}

JSONBase * JSONBase::getInstance() {
	if (instance == NULL) {
		instance = new JSONBase();
	}
	return instance;
}

bool JSONBase::initialize() {
	if (this->is_initialized == true) {
		return false;
	}
	this->is_initialized = true;

	void ** static_spaces;
	static_spaces = get_static_spaces();

	if (static_spaces[8] == NULL) {
		this->dm_hashTable = newDMHashTable();
		this->dm_hashTable_space = dm_hashTable->self;
		static_spaces[8] = dm_hashTable->self;

	} else {
		this->dm_hashTable_space = (MemorySpace *) static_spaces[8];
		this->dm_hashTable = (DMHashTable *) this->dm_hashTable_space->pointer;
	}

	if (static_spaces[9] == NULL) {
		this->connection_pool = newJSONBaseConnectionPool();
		this->connection_pool_space = connection_pool->self;
		static_spaces[9] = connection_pool->self;

	} else {
		this->connection_pool_space = (MemorySpace *) static_spaces[9];
		this->connection_pool = (JSONBaseConnectionPool *) this->dm_hashTable_space->pointer;
	}

	return true;
}

RootNode * JSONBase::create(DMString * node_name, DMString * secret) {
	this->dm_hashTable = (DMHashTable *) this->dm_hashTable_space->pointer;
	MemorySpace * check_exist = this->dm_hashTable->get((MemorySpace *) node_name);
	if (check_exist != NULL) {
		return NULL;
	}

	RootNode * root_node = newRootNode();

	root_node->node_name = newDMString(node_name->max_length);
	root_node->node_name->copy(node_name->char_string, node_name->max_length);
	root_node->secret = newDMString(secret->max_length);
	root_node->secret->copy(secret->char_string, secret->max_length);

	root_node->access_key = newDMString(40);
	char * access_key_char_string = sha1(root_node->access_key, 12);
	root_node->access_key->copy(access_key_char_string, 40);

	root_node->id = this->dm_hashTable->length + 0x80088000;

	root_node->root = newDMJSON();
	root_node->root_space = root_node->root->self;

	DMKeyValue * dm_key_value = this->dm_hashTable->set((MemorySpace *) root_node->node_name, root_node->self);

	return root_node;
}

void JSONBase::list(TraverseCallback * traverseCallback) {
	this->dm_hashTable = (DMHashTable *) this->dm_hashTable_space->pointer;
	this->dm_hashTable->traverse(traverseCallback);
	return;
}

RootNode * JSONBase::getRootNode(DMString * node_name, DMString * access_key) {
	this->dm_hashTable = (DMHashTable *) this->dm_hashTable_space->pointer;
	MemorySpace * root_node_space = this->dm_hashTable->get((MemorySpace *) node_name);
	if (root_node_space == NULL) {
		return NULL;
	}
	RootNode * root_node = (RootNode *) root_node_space->pointer;
	if (!compareDMString(access_key, root_node->access_key)) {
		if (!compareDMString(access_key, root_node->secret)) {
			return NULL;
		}
	}
	return root_node;
}

JSONBaseConnection * JSONBase::open(DMString * node_name, DMString * access_key) {
	RootNode * root_node = this->getRootNode(node_name, access_key);
	if (root_node == NULL) {
		return NULL;
	}
	this->connection_pool = (JSONBaseConnectionPool *) this->connection_pool_space->pointer;

	this->connection_pool->connection_pool_start = (JSONBaseConnection *) this->connection_pool->connection_pool_start_space->pointer;
	JSONBaseConnection * new_connection = this->connection_pool->connection_pool_start + (this->connection_pool->current_connection_id % this->connection_pool->connection_pool_size);

	new_connection->id = this->connection_pool->current_connection_id;
	this->connection_pool->current_connection_id++;

	if (new_connection->access_key != NULL) {
		freeDMString(new_connection->access_key);
	}
	new_connection->access_key = newDMString(40);

	char * access_key_char_string = sha1(new_connection->access_key, 12);
	new_connection->access_key->copy(access_key_char_string, 40);

	if (new_connection->root_node_access_key == NULL) {
		new_connection->root_node_access_key = newDMString(40);
	}
	new_connection->root_node_access_key->copy(root_node->access_key->char_string, 40);

	if (new_connection->current_path == NULL) {
		new_connection->current_path = newDMString(64);
	}
	new_connection->root_node_space = root_node->self;
	new_connection->root_node = root_node;
	new_connection->current_space = root_node->root_space;

	return new_connection;

}

bool JSONBase::close(int connection_id, DMString * access_key) {
	this->connection_pool = (JSONBaseConnectionPool *) this->connection_pool_space->pointer;

	this->connection_pool->connection_pool_start = (JSONBaseConnection *) this->connection_pool->connection_pool_start_space->pointer;
	JSONBaseConnection * connection = this->connection_pool->connection_pool_start + (connection_id % this->connection_pool->connection_pool_size);
	if (!compareDMString(access_key, connection->access_key)) {
		return false;
	}
	connection->id = 0;
	connection->root_node_space = NULL;
	connection->current_space = NULL;
	return true;
}

JSONBaseConnection * JSONBase::getConnection(int connection_id, DMString * access_key) {
	this->connection_pool = (JSONBaseConnectionPool *) this->connection_pool_space->pointer;

	this->connection_pool->connection_pool_start = (JSONBaseConnection *) this->connection_pool->connection_pool_start_space->pointer;
	JSONBaseConnection * connection = this->connection_pool->connection_pool_start + (connection_id % this->connection_pool->connection_pool_size);
	if (connection->id != connection_id) {
		return NULL;
	}
	if (!compareDMString(access_key, connection->access_key)) {
		return NULL;
	}
	connection->root_node = (RootNode *) connection->root_node_space->pointer;
	if (!compareDMString(connection->root_node->access_key, connection->root_node_access_key)) {
		return NULL;
	}
	return connection;
}

bool RootNode::put(DMString * key_chain, MemorySpace * value) {
	this->root = (DMJSON *) this->root_space->pointer;
	return this->root->put(key_chain, value);
}

bool RootNode::set(DMString * key_chain, MemorySpace * value) {
	this->root = (DMJSON *) this->root_space->pointer;
	return this->root->set(key_chain, value);
}

MemorySpace * RootNode::get(DMString * key_chain) {
	this->root = (DMJSON *) this->root_space->pointer;
	return this->root->get(key_chain);
}

MemorySpace * RootNode::del(DMString * key_chain) {
	this->root = (DMJSON *) this->root_space->pointer;
	return this->root->del(key_chain);
}

bool JSONBaseConnection::put(DMString * key_chain, MemorySpace * value) {
	if ((*key_chain) == "this") {
		return false;

	} else {
		if (this->current_space->type == TYPE_JSON) {
			this->current_json = (DMJSON *) this->current_space->pointer;
			return this->current_json->put(key_chain, value);
		} else {
			return false;
		}
	}
}

bool JSONBaseConnection::set(DMString * key_chain, MemorySpace * value) {
	if ((*key_chain) == "this") {
		return exchangeSpace(this->current_space, value);
	} else {
		if (this->current_space->type != TYPE_JSON) {
			DMJSON * new_json = newDMJSON();
			new_json->set(-1, new_json->self, NOT_REPLACE);
			exchangeSpace(this->current_space, new_json->self);
		}
		this->current_json = (DMJSON *) this->current_space->pointer;
		return this->current_json->set(key_chain, value);
	}
}

MemorySpace * JSONBaseConnection::get(DMString * key_chain) {
	if ((*key_chain) == "this") {
		return this->current_space;
	} else {
		if (this->current_space->type == TYPE_JSON) {
			this->current_json = (DMJSON *) this->current_space->pointer;
			return this->current_json->get(key_chain);
		} else {
			return NULL;
		}
	}
}

MemorySpace * JSONBaseConnection::del(DMString * key_chain) {
	if ((*key_chain) == "this") {
		return NULL;
	} else {
		if (this->current_space->type == TYPE_JSON) {
			this->current_json = (DMJSON *) this->current_space->pointer;
			return this->current_json->del(key_chain);
		} else {
			return NULL;
		}
	}

}

bool JSONBaseConnection::cd(DMString * key_chain) {
	if ((*key_chain) == "root") {
		this->current_path->used_length = 0;
		this->root_node = (RootNode *) this->root_node_space->pointer;
		this->current_space = this->root_node->root_space;
		return true;
	}

	if ((*key_chain) == "..") {
		char localChar;
		for (int i = this->current_path->used_length - 1; i >= 0; i--) {
			localChar = this->current_path->char_string[i];
			if (localChar == '.' || localChar == '#' || i == 0) {
				this->current_path->used_length = i;
				break;
			}
		}
		this->root_node = (RootNode *) this->root_node_space->pointer;
		this->root_node->root = (DMJSON *) this->root_node->root_space->pointer;
		MemorySpace * target_space = this->root_node->root->get(this->current_path);
		if (target_space == NULL) {
			return false;
		}

		this->current_space = target_space;
		return true;
	}

	if (this->current_space->type != TYPE_JSON) {
		return false;
	}
	this->current_json = (DMJSON *) this->current_space->pointer;
	MemorySpace * target_space = this->current_json->get(key_chain);

	if (target_space != NULL && target_space->type == TYPE_KEY_VALUE) {
		DMKeyValue * dm_key_value = (DMKeyValue *) target_space->pointer;
		target_space = dm_key_value->value;
	}

	if (target_space == NULL) {
		return false;
	}

	this->current_space = target_space;
	if (this->current_path->used_length > 0) {
		if (key_chain->char_string[0] == '#') {
			(*this->current_path) * "@@" % key_chain;
		} else {
			(*this->current_path) * ".@@" % key_chain;
		}
	} else {
		(*this->current_path) * "@@" % key_chain;
	}
	return true;
}

MemorySpace * JSONBaseConnection::list() {
	return this->current_space;
}

//DMString * jsonbase_shell_output = NULL;

void onGetData_RootNodes(MemorySpace * data_space) {
	JSONBase * json_base = JSONBase::getInstance();
	DMString * list_output = json_base->list_output;
	if (data_space == NULL || list_output == NULL) {
		return;
	}
	DMKeyValue * key_value = (DMKeyValue *) data_space->pointer;
	DMString * key = (DMString *) key_value->key;
	key->used_length = key->max_length;
	(*list_output) * "@@  " % key;
}

void onGetData_NodeElements(MemorySpace * data_space) {
	JSONBase * json_base = JSONBase::getInstance();
	DMString * list_output = json_base->list_output;
	if (data_space == NULL || list_output == NULL) {
		return;
	}

	if (list_output->used_length > 1) {
		(*list_output) * " ";
	}

	if (data_space->type == TYPE_STRING) {
		(*list_output) * "str";
	} else if (data_space->type == TYPE_NUMBER) {
		(*list_output) * "num";
	} else if (data_space->type == TYPE_KEY_VALUE) {
		DMKeyValue * key_value = (DMKeyValue *) data_space->pointer;
		DMString * key = (DMString *) key_value->key;
		MemorySpace * value = key_value->value;
		key->used_length = key->max_length;
		(*list_output) * "@@:" % key;

		if (value->type == TYPE_STRING) {
			(*list_output) * "str";
		} else if (value->type == TYPE_NUMBER) {
			(*list_output) * "num";
		} else if (value->type == TYPE_KEY_VALUE) {
			(*list_output) * "error";
		} else if (value->type == TYPE_JSON) {
			(*list_output) * "json";
		}
	} else if (data_space->type == TYPE_JSON) {
		(*list_output) * "json";
	}
}
