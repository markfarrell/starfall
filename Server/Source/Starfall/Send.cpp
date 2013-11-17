//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).

#include "Starfall/Send.h"

#include "Starfall/Head.h"
#include "Starfall/ObjectsUpdateStruct.h"

#include <Poco/NumberFormatter.h>

#include <iostream>
#include <vector>

using namespace Starfall;

using std::vector;
using std::cout;
using std::endl;

std::map<SendFunction, Poco::UInt32> Send::Map;

bool Send::Enqueue(Player::Ptr& pPlayer, SendFunction caller, Buffer& body) {
	try {

		Head head;
		head.begin = 0xFFFFFFFF;
		head.bodysize = body.size();
		head.opcode = Send::Map.at(caller);
		head.end = 0xFFFFFFFF;
		Buffer buffer;
		buffer << head;

		buffer.insert(buffer.end(), body.begin(), body.end());
		pPlayer->sendQueue.push(buffer);
		return true;
	} catch(std::out_of_range& oor) {
		std::cerr << "[Send::Enqueue] Out of Range error: " << oor.what() << '\n'; 
	}
	return false;
}

void Send::Init() {
	Send::Map[&Send::LoginReply] = 0x10000000 | 0x01;
	Send::Map[&Send::CreateEntityData] = 0x10000000 | 0x03;
	Send::Map[&Send::TransformEntityData] = 0x10000000 | 0x04;
	//Send::Map[&Send::DeleteEntityData] = 0x10000000 | 0x05;
	Send::Map[&Send::ObjectsReply] = 0x10000000 | 0x06;

}

bool Send::LoginReply(Player::Ptr& pPlayer) {
	Buffer buffer;
	LoginStruct loginStruct;
	loginStruct.state = pPlayer->state;
	loginStruct.userid = pPlayer->userid;
	loginStruct.usertype = pPlayer->usertype;
	loginStruct.username = pPlayer->username;
	loginStruct.password = pPlayer->password;

	buffer << loginStruct;

	return Send::Enqueue(pPlayer, &Send::LoginReply, buffer);
}

bool Send::CreateEntityData(Player::Ptr& pPlayer) { 
	Buffer buffer;
	
	buffer << pPlayer->createEntityQueue;

	return Send::Enqueue(pPlayer, &Send::CreateEntityData, buffer);
}

bool Send::TransformEntityData(Player::Ptr& pPlayer) { 
	Buffer buffer;

	buffer << pPlayer->transformEntityQueue;

	return Send::Enqueue(pPlayer, &Send::TransformEntityData, buffer);
}

bool Send::ObjectsReply(Player::Ptr& pPlayer) {
	Buffer buffer;

	ObjectsUpdateStruct updateStruct; //allocate the update struct on the heap since the number of entities being created and deleted is uncertain.

	updateStruct.state = pPlayer->updateState; //allow another objects data packet to be sent
	updateStruct.farClipDistance = pPlayer->farClipDistance;
	updateStruct.createEntities = pPlayer->createEntityQueue; //Copy operation is acceptable to fit design pattern: Data structures sent over the network should be contained within their own block of memory.
	updateStruct.destroyEntities = pPlayer->destroyEntityQueue;

	buffer << updateStruct; 

	return Send::Enqueue(pPlayer, &Send::ObjectsReply, buffer);
}