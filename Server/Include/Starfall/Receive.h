//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).


#pragma once

#include "Poco/Net/Net.h"

#include <map>

#include "Starfall/Head.h"
#include "Starfall/Buffer.h"
#include "Starfall/Player.h"

namespace Starfall {

	typedef bool (*ReceiveFunction) (Player::Ptr& pPlayer, Buffer& buffer, Packet<Head>& head);
	class Receive {
		friend class LoginServer;
		friend class LoginConnectionHandler;
		protected:
			static std::map<Poco::UInt32, ReceiveFunction> Map;
		public:
			static void Init(); //Called in main
			static bool LoginData(Player::Ptr& pPlayer, Buffer& buffer, Packet<Head>& head);
			static bool ConsoleData(Player::Ptr& pPlayer, Buffer& buffer, Packet<Head>& head);
			static bool TransformData(Player::Ptr& pPlayer, Buffer& buffer, Packet<Head>& head);
			static bool ObjectsData(Player::Ptr& pPlayer, Buffer& buffer, Packet<Head>& head);
	};

}
