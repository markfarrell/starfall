#pragma once

#include "Poco/Net/Net.h"

#include <map>

#include "Buffer.h"
#include "Player.h"

typedef bool (*SendFunction) (Player::Ptr& pPlayer);
class Send {
	friend class LoginServer;
	private:
		static bool Enqueue(Player::Ptr& pPlayer, SendFunction caller, Buffer& body);
	protected:
		static std::map<SendFunction, Poco::UInt32> Map; //Note: The key-value order is reversed for send!
	public:
		static void Init();
		static bool LoginReply(Player::Ptr& pPlayer);
		static bool CreateEntityData(Player::Ptr& pPlayer);
		static bool TransformEntityData(Player::Ptr& pPlayer);
		static bool ObjectsReply(Player::Ptr& pPlayer);
};