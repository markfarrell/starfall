//Copyright (c) 2013 Mark Farrell
#pragma once

#include "Starfall/User.h"

#include <SFML/System/Utf.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>

#include <Poco/SharedPtr.h>

#include <vector>

namespace Starfall {
	class Client {
		private:
			typedef Poco::SharedPtr<Client> Ptr; //allocate space for clients on the heap so many can be instantiated for benchmarking

			sf::TcpSocket socket;
			User user;
				
			Client();

			static std::vector<Client::Ptr> clients; //all clients will be deleted when the program ends automatically.
		public:
			static Client::Ptr Get(); //get the main client for usage by the application, clients[0]
	};
}