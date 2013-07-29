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
