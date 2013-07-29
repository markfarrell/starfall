//The MIT License (MIT)
//
//Copyright (c) 2013 Mark Farrell
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.


#include "Starfall/Player.h"

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
			if(Position::Distance(this->pEntity->position, createEntityStruct.position) <= farClipDistance) //Receive methods are thread safe for pPlayer
			{
				this->createEntityQueue.push_back(createEntityStruct); //uses sharedptr
			}

		} catch ( Poco::NotFoundException e ) {
			cout << e.what() << endl;
		}
}