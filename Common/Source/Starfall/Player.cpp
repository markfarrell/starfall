//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).

#include "Starfall/Player.h"

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include <iostream>

using namespace Starfall;

using std::cout;
using std::endl;

Player::Ptr Player::Create(string address, Poco::UInt32 sessionid) { 
	return Player::Ptr(new Player(address, sessionid));
}

Player::Player(string address, Poco::UInt32 sessionid) : User(address) {

	this->farClipDistance = 0.0;
	this->updateState = 0;

	this->pEntity = Entity::Create(sessionid);
	this->pEntity->displayName = "Player";
	this->pEntity->mode = 2; //Controlled by camera

}

Player::~Player() {
	Entity::Remove(this->pEntity->sessionid);
}

void Player::lock() {
	this->pEntity->mutex.lock();
}

void Player::unlock() {
	this->pEntity->mutex.unlock();
}

Poco::Mutex& Player::mutex() {
	return this->pEntity->mutex;
}

void Player::destroyEntity(Poco::UInt32 sessionid) {
		DestroyEntityStruct destroyEntityStruct;
		destroyEntityStruct.sessionid = sessionid;
		this->destroyEntityQueue.push_back(destroyEntityStruct); //uses sharedptr
}

void Player::transformEntity(Poco::UInt32 sessionid) {
	try {
		TransformEntityStruct transformEntityStruct = Entity::FindTransformEntityStruct(sessionid); //Thread safe retrieval of entity information
		this->transformEntityQueue.push_back(transformEntityStruct); //uses sharedptr
	} catch ( Poco::NotFoundException e ) {
		cout << e.what() << endl;
	}
}

void Player::createEntity(Poco::UInt32 sessionid) {
	try {
			CreateEntityStruct createEntityStruct = Entity::FindCreateEntityStruct(sessionid); //Thread safe retrieval of entity information

			if(float(glm::length2(this->pEntity->position-createEntityStruct.position)) <= float(pow(farClipDistance,2))) //Receive methods are thread safe for pPlayer
			{
				this->createEntityQueue.push_back(createEntityStruct); //uses sharedptr
			}

		} catch ( Poco::NotFoundException e ) {
			cout << e.what() << endl;
		}
}