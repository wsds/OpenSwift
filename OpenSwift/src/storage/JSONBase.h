#ifndef JSONBASE_H
#define JSONBASE_H

#include "../DM/DMJSONHelper.h"
#include "../lib/LibSha1.h"

class RootNode {
public:
	MemorySpace * self;

	int id;
	DMJSON * root;
	MemorySpace * root_space;

	DMString * secret;
	DMString * access_key;

	DMString * node_name;

	bool put(DMString * key_chain, MemorySpace * value);
	bool set(DMString * key_chain, MemorySpace * value);
	MemorySpace * get(DMString * key_chain);
	MemorySpace * del(DMString * key_chain);

};
RootNode * newRootNode();

class JSONBaseConnection {
public:
	int id;
	DMString * access_key;
	DMString * root_node_access_key;

	DMJSON * current_json;
	MemorySpace * current_space;
	DMString * current_path;

	RootNode * root_node;
	MemorySpace * root_node_space;

	bool put(DMString * key_chain, MemorySpace * value);
	bool set(DMString * key_chain, MemorySpace * value);
	MemorySpace * get(DMString * key_chain);
	MemorySpace * del(DMString * key_chain);

	bool cd(DMString * key_chain);
	MemorySpace * list();

};

class JSONBaseConnectionPool {
public:
	MemorySpace * self;

	MemorySpace * connection_pool_start_space;
	JSONBaseConnection * connection_pool_start;

	int connection_pool_size;
	int current_connection_id;
};

JSONBaseConnectionPool * newJSONBaseConnectionPool();

class JSONBase {
public:

	JSONBase();
	bool is_initialized;
	static JSONBase * instance;
	static JSONBase * getInstance();
	bool initialize();

	MemorySpace * dm_hashTable_space;
	DMHashTable * dm_hashTable;

	MemorySpace * connection_pool_space;
	JSONBaseConnectionPool * connection_pool;

	int error_number;

	RootNode * create(DMString * node_name, DMString * secret);
	void list(TraverseCallback * traverseCallback);
	RootNode * getRootNode(DMString * node_name, DMString * access_key);

	JSONBaseConnection * open(DMString * node_name, DMString * access_key);
	bool close(int connection_id, DMString * access_key);
	JSONBaseConnection * getConnection(int connection_id, DMString * access_key);

	DMString * list_output;

};




void onGetData_RootNodes(MemorySpace * data_space);

void onGetData_NodeElements(MemorySpace * data_space);

#endif /* JSONBASE_H */
