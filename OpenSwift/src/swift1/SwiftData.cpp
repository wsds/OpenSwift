#include "SwiftData.h"


JSONPool * JSONPool::instance = NULL;
JSONPool * JSONPool::getInstance() {
	if (instance == NULL) {
		instance = new JSONPool();

	}
	return instance;
}

JSONPool::JSONPool() {
	int pool_size = 512;
	this->json_pool = newDMLIST_atomic();
	this->json_id = 0;
	DMJSON * initialize_jsons[pool_size];
	for (int i = 0; i < pool_size; i++) {
		initialize_jsons[i] = this->getDMJSON();
	}
	for (int i = 0; i < pool_size; i++) {
		this->freeDMJSON(initialize_jsons[i]);
	}
}

DMJSON * JSONPool::getDMJSON() {
	MemorySpace * json_space = this->json_pool->leftPop_atomic();
	if (json_space == NULL) {
		json_space = newDMJSON()->self;
	}

	DMJSON * json = (DMJSON *) json_space->pointer;
	this->json_id++;
	return json;
}



void JSONPool::freeDMJSON(DMJSON * json) {
	this->json_pool->rightPush_atomic(json->self);
}

SwiftData::SwiftData() {

}

