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


#include "Starfall/Send.h"
#include "Starfall/Head.h"

#include "Poco/NumberFormatter.h"
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
		buffer.writeUInt32(head.begin);
		buffer.writeUInt32(head.bodysize);
		buffer.writeUInt32(head.opcode);
		buffer.writeUInt32(head.end);
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
	buffer.writeUInt32(pPlayer->state);
	buffer.writeUInt32(pPlayer->userid);
	buffer.writeUInt32(pPlayer->usertype);
	buffer.writeString(pPlayer->username);
	buffer.writeString(pPlayer->password);
	return Send::Enqueue(pPlayer, &Send::LoginReply, buffer);
}

/** 
 * Note: This function does not alter the contents of the player's create entity queue. 
 */
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
	buffer.writeUInt32(1); //set state; allow another objectsData packet to be sent.
	buffer << pPlayer->createEntityQueue; 
	buffer << pPlayer->destroyEntityQueue;
	return Send::Enqueue(pPlayer, &Send::ObjectsReply, buffer);
}