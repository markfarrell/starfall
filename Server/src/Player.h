#pragma once

#include "Poco/SharedPtr.h"

#include "User.h"
#include "LoginStruct.h"
#include "Entity.h"
#include <string>

using std::string;

class Player : protected User, protected LoginStruct {

	friend class LoginServer;
	friend class LoginConnectionHandler;
	friend class Receive;
	friend class Send;

	public:
		typedef Poco::SharedPtr<Player> Ptr;
		~Player(); 
	protected:

		Entity::Ptr pEntity;
		double farClipDistance; //the last clip distance received in a packet

		static Ptr create(string address); 

		void lock();
		void unlock();
		void destroyEntity(Poco::UInt32 sessionid);
		void createEntity(Poco::UInt32 sessionid);
		void transformEntity(Poco::UInt32 sessionid);

	private:
		Player(string address);

};