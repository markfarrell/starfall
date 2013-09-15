//Copyright (c) 2013 Mark Farrell

#include "Starfall/Entity.h"

#include "Starfall/Isolates.h"
#include "Starfall/Config.h"

#include <iostream>
#include <limits>

using namespace Starfall;

Poco::Mutex  Entity::CollectionMutex;
Poco::HashMap<Poco::UInt32, Entity::Ptr> Entity::Map;

Entity::Entity(Poco::UInt32 sessionid) {
	this->isolate = Isolates::Acquire();
	this->persistentContext = IO::Context();
	this->displayName = "";
	this->mode = 0;
	this->sessionid = sessionid;
	this->Load();
}

void Entity::Load() {
	v8::HandleScope handleScope(this->isolate); 
	v8::Context::Scope contextScope(this->persistentContext); 
	//v8::Handle<v8::Value> result = IO::Run("Script/Entity/context.js");
}

Entity::~Entity() {
	this->persistentContext.Dispose(); //entity's global context must be deleted manually
	Isolates::Release(this->isolate);
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
	createEntityStruct.orientation = this->orientation;
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



Entity::Ptr Entity::Create(Poco::UInt32 sessionid) { 
	Entity::Ptr pEntity = Entity::Ptr(new Entity(sessionid));
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