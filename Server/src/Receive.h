#pragma once

#include "Poco/Net/Net.h"

#include <map>

#include "Head.h"
#include "Buffer.h"
#include "Player.h"

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
