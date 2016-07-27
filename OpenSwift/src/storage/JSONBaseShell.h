#ifndef JSONBASESHELL_H
#define JSONBASESHELL_H
#include <stdio.h>
#include "JSONBase.h"
#include "../lib/DMLog.h"

class JSONBaseShell {
public:

	JSONBaseShell();
	bool is_initialized;
	static JSONBaseShell * instance;
	static JSONBaseShell * getInstance();
	bool initialize();

	DMString * input;
	DMString * output;

	DMString * root_node_name;
	DMString * root_node_access_key;
	RootNode * root_node;
	MemorySpace * root_node_space;

	int connection_id;
	JSONBaseConnection * connection;
	DMString * connection_access_key;
	DMString * current_path;

	DMString * prompt;

	TraverseCallback * list_root_nodes;
	TraverseCallback * list_node_elements;


	void shell();
};

#endif /* JSONBASESHELL_H */
