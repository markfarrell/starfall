//
//
//Copyright (c) 2013 Mark Farrell
//
//





//


//









#include "Poco/Foundation.h"

#include "Poco/Data/Common.h"
#include "Poco/Data/SQLite/Connector.h"
#include "Poco/Exception.h"

#include "Poco/JSON/Object.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/Dynamic/Struct.h"

#include <iostream>
#include <set>
#include <vector>
#include <algorithm>

#include "Starfall/Receive.h"
#include "Starfall/Send.h"
#include "Starfall/Database.h"
#include "Starfall/Console.h"
#include "Starfall/Packet.h"
#include "Starfall/Entity.h"

using namespace Starfall;

using std::copy;
using std::vector;
using std::cout;
using std::endl;

std::map<Poco::UInt32, ReceiveFunction> Receive::Map;

void Receive::Init() { 
	Receive::Map[0x01] = &Receive::LoginData;
	Receive::Map[0x02] = &Receive::ConsoleData;
	Receive::Map[0x04] = &Receive::TransformData;
	Receive::Map[0x06] = &Receive::ObjectsData;
}

bool Receive::LoginData(Player::Ptr& pPlayer, Buffer& buffer, Packet<Head>& head) {

	Packet<LoginStruct> loginPacket;
	buffer >> loginPacket;
	http::out << head << loginPacket;

	if(pPlayer->userid != loginPacket->userid) {
		std::cerr << "Error: The integer read from Receives::Login does not match the default userid! Address: " << pPlayer->address << endl;
		return false;
	}
	pPlayer->state = loginPacket->state;
	pPlayer->usertype = loginPacket->usertype;
	pPlayer->username = loginPacket->username;
	pPlayer->password = loginPacket->password;
	cout << "Username: " << pPlayer->username << ", Password: " << pPlayer->password << ", Address: " << pPlayer->address << endl;

	LoginStruct& loginStruct = loginPacket.value();
	if(Database::Select::TryLogin(loginStruct)) {
		pPlayer->state = 1; //logging in
		pPlayer->userid  = loginStruct.userid;
		pPlayer->usertype = loginStruct.usertype;
		pPlayer->username = loginStruct.username;
		pPlayer->password = loginStruct.password;
	}

	if(!Send::LoginReply(pPlayer)) { return false; }

	//TODO: Load additional entity data from database: including position
	pPlayer->pEntity->displayName = pPlayer->username;

	pPlayer->createEntityQueue.push_back(pPlayer->pEntity->castCreateEntityStruct());
	if(!Send::CreateEntityData(pPlayer)) { return false; }
	pPlayer->createEntityQueue.clear();

	return true; 
}


bool Receive::ConsoleData(Player::Ptr& pPlayer, Buffer& buffer, Packet<Head>& head) {
	Poco::UInt32 nMessages = buffer.readUInt32();
	for(Poco::UInt32 i = 0; i < nMessages; i++) {
		try
		{
			string data = buffer.readString();

			Poco::Dynamic::Var var = Poco::Dynamic::Var::parse(data);

			JSONConsoleStruct json;
			json.copy(var);

			Console::Print(json.zip()); //zip applies datetime

		}catch(ConsoleException e) {
			std::cerr << e.what() << endl;
			return false;
		}
	}
	return true;
}

bool Receive::TransformData(Player::Ptr& pPlayer, Buffer& buffer, Packet<Head>& head) { 
	Packet<vector<TransformStruct>> transformsPacket;
	buffer >> transformsPacket;
	//The entity's actual position is the most recent position; it is the last element in the path collection
	if(transformsPacket->size() > 0) {
		pPlayer->pEntity->position = transformsPacket->back().position;
		pPlayer->pEntity->rotation = transformsPacket->back().rotation;
	}
	pPlayer->pEntity->clearPath();
	//Add the path nodes to the player's entity's transform queue.
	for(vector<TransformStruct>::iterator it = transformsPacket->begin(); it != transformsPacket->end(); it++) {
		pPlayer->pEntity->addToPath((*it));
	}
	return true;
}

bool Receive::ObjectsData(Player::Ptr& pPlayer, Buffer& buffer, Packet<Head>& head) {

	Poco::UInt32 clientUpdateState = buffer.readUInt32(); //client waits until it receives a response to send another request
	pPlayer->farClipDistance = buffer.readFloat();
	Packet<vector<Poco::UInt32>> objectsPacket;
	buffer >> objectsPacket;

	vector<Poco::UInt32> keys = Entity::Keys();

	for(vector<Poco::UInt32>::iterator keysIterator = keys.begin(); keysIterator != keys.end(); keysIterator++) {
		Poco::UInt32 entityState = 0; //create
		for(vector<Poco::UInt32>::iterator it = objectsPacket->begin(); it != objectsPacket->end(); it++) {

			if(std::find(keys.begin(), keys.end(), (*it)) == keys.end()) { //server does not have the entity anymore
				entityState = 1; //delete
				pPlayer->destroyEntity((*it));
			}

			if((*it) == (*keysIterator)) { //both client and server have the entity
				entityState = 2; // transform
				pPlayer->transformEntity((*it));
			}
		}

		if(entityState == 0) { //server but not the client has the entity
			pPlayer->createEntity((*keysIterator));
		}

	}

	if(!(Send::ObjectsReply(pPlayer) && Send::TransformEntityData(pPlayer))) { return false; }

	pPlayer->clearAll();

	
	return true;
}



