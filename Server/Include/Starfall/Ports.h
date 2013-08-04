//
//
//Copyright (c) 2013 Mark Farrell
//
//





//


//








#pragma once
#include "Poco/Net/Net.h"

namespace Starfall {
	/**
	 * Date: April 24th, 2013
	 * Description: Outlines all of the ports the application will be using and where..
	 * TODO: Create tables in the DB for users between all servers.
	 */
	class Ports {
		friend class LoginServer;
		private:
			static Poco::UInt16 ToLoginServerFromClient() { return 12777; }
			static Poco::UInt16 ToLoginServerFromWeb() { return 12778; }
			static Poco::UInt16 ToGameServerFromWeb() { return 12779; }
			static Poco::UInt16 ToLoginServerFromGameServer() { return 12780; }
			static Poco::UInt16 ToGameServerFromLoginServer() { return 12781; }
	};

}
