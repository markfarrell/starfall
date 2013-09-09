//Copyright (c) 2013 Mark Farrell

#include "Starfall/Player.h"

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include <iostream>

using namespace Starfall;

using std::cout;
using std::endl;

Player::Ptr Player::create(string address) { 
	return Player::Ptr(new Player(address));
}

Player::Player(string address) : User(address) {
	this->pEntity = Entity::Create();
	this->pEntity->displayName = "Player";
	this->pEntity->mode = 2; //Controlled by camera
	this->farClipDistance = 0.0;
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