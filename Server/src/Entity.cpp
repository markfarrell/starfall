#include "Entity.h"
#include "Config.h"
#include <iostream>
#include <limits>

Poco::UInt32 IDGenerator::id = 0;
Poco::Mutex  IDGenerator::mutex;
Poco::Mutex  Entity::CollectionMutex;
Poco::HashMap<Poco::UInt32, Entity::Ptr> Entity::Map;

DestroyEntityStruct::DestroyEntityStruct() {
	this->sessionid = 0;
}

CreateEntityStruct::CreateEntityStruct() {
	this->sessionid = 0;
	this->mode = 0;
	this->displayName = "";
	this->appearance = "";
}

TransformStruct::TransformStruct() {
	this->action = 0;
}

TransformEntityStruct::TransformEntityStruct() {
	this->sessionid = 0;
}

Poco::UInt32 IDGenerator::next() {	
	Poco::UInt32 nextID = 0;
	IDGenerator::mutex.lock();
	IDGenerator::id++;
	nextID = IDGenerator::id;
	IDGenerator::mutex.unlock();
	return nextID;
}

Entity::Entity() {
	this->displayName = "";
	//this->appearance = Config::Appearance();
	this->mode = 0;
	this->sessionid = IDGenerator::next();
}

void Entity::addToPath(TransformStruct transformStruct) {
	this->mutex.lock();
	this->path.push_back(transformStruct);
	this->mutex.unlock();
}

void Entity::clearPath() {
	this->mutex.lock();
	this->path.clear();
	this->mutex.unlock();
}


CreateEntityStruct Entity::castCreateEntityStruct() {
	CreateEntityStruct createEntityStruct; 
	createEntityStruct.sessionid = this->sessionid;
	createEntityStruct.displayName = this->displayName;
	createEntityStruct.mode = this->mode;
	createEntityStruct.position = this->position;
	createEntityStruct.rotation = this->rotation;
	//createEntityStruct.appearance = this->appearance;
	return createEntityStruct;
}

TransformEntityStruct Entity::castTransformEntityStruct() {
	TransformEntityStruct transformEntityStruct; 
	transformEntityStruct.sessionid = this->sessionid;
	for(vector<TransformStruct>::iterator it = this->path.begin(); it != this->path.end(); it++) {
		transformEntityStruct.path.push_back((*it));
	}
	return transformEntityStruct;
}



Entity::Ptr Entity::Create() { 
	Entity::Ptr pEntity = Entity::Ptr(new Entity());
	Entity::CollectionMutex.lock(); //The map isn't thread safe; so we must claim ownership when accessing it.
	Entity::Map[pEntity->sessionid] = pEntity; 
	Entity::CollectionMutex.unlock(); //Release ownership
	return pEntity;
}

vector<Poco::UInt32> Entity::Keys() {
	vector<Poco::UInt32> keys;
	Entity::CollectionMutex.lock();
	for(Poco::HashMap<Poco::UInt32, Entity::Ptr>::Iterator it = Entity::Map.begin(); it != Entity::Map.end(); it++) {
		keys.push_back(it->first);
	}
	Entity::CollectionMutex.unlock();
	return keys;
}


CreateEntityStruct Entity::FindCreateEntityStruct(Poco::UInt32 sessionid) {
	CreateEntityStruct createEntityStruct;
	Entity::Ptr pEntity;
	Entity::CollectionMutex.lock();
	pEntity = Entity::Map[sessionid]; //Note: unlike std::map, Poco::HashMap throws a Poco::NotFoundException that we can use.
	Entity::CollectionMutex.unlock();
	pEntity->mutex.lock(); //lock access to entity's information.
	createEntityStruct = pEntity->castCreateEntityStruct();
	pEntity->mutex.unlock();
	return createEntityStruct;
}

TransformEntityStruct Entity::FindTransformEntityStruct(Poco::UInt32 sessionid) {
	TransformEntityStruct transformEntityStruct;
	Entity::Ptr pEntity;
	Entity::CollectionMutex.lock();
	pEntity = Entity::Map[sessionid]; //Note: unlike std::map, Poco::HashMap throws a Poco::NotFoundException that we can use.
	Entity::CollectionMutex.unlock();
	pEntity->mutex.lock(); //lock access to entity's information.
	transformEntityStruct = pEntity->castTransformEntityStruct();
	pEntity->mutex.unlock();
	return transformEntityStruct;
}

void Entity::Remove(Poco::UInt32 sessionid) {	
	Entity::CollectionMutex.lock();
	Entity::Map.erase(sessionid); //fast erase with a hash
	Entity::CollectionMutex.unlock();
}