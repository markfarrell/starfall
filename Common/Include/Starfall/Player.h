//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).

#pragma once

#include "Starfall/User.h"
#include "Starfall/LoginStruct.h"
#include "Starfall/Entity.h"
#include "Starfall/IDGenerator.h"

#include <Poco/SharedPtr.h>
#include <Poco/Mutex.h>

#include <string>

using std::string;

namespace Starfall {

	class Player : protected User, protected LoginStruct {

		friend class LoginServer;
		friend class LoginConnectionHandler;
		friend class Receive;
		friend class Send;

		friend class Client;
		friend class ClientSender;
		friend class ClientReceiver;

		friend class WorldScene;

		public:
			typedef Poco::SharedPtr<Player> Ptr;
			~Player(); 
		protected:

			Poco::UInt32 updateState; //0=can update, 1=cannot update

			Entity::Ptr pEntity;

			float farClipDistance; //the last clip distance received in a packet

			static Ptr Create(string address, Poco::UInt32 sessionid=IDGenerator::Next()); 

			void lock();
			void unlock();
			Poco::Mutex& mutex();

			void destroyEntity(Poco::UInt32 sessionid);
			void createEntity(Poco::UInt32 sessionid);
			void transformEntity(Poco::UInt32 sessionid);

		private:
			Player(string address, Poco::UInt32 sessionid);

	};

}