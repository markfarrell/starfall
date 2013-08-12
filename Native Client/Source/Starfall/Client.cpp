//Copyright (c) 2013 Mark Farrell
#include "Starfall\Client.h"

#include "Starfall\ConfigurationFile.h"

#include <string>

using std::string;

using namespace Starfall;

std::vector<Client::Ptr> Client::clients;

Client::Client() : user(ConfigurationFile::Client().getString("server.address")+string(":")+ConfigurationFile::Client().getString("server.port")) {} //User address is set based on INET address format using config

Client::Ptr Client::Get() {
	if(Client::clients.size() == 0) {
		clients.push_back(Client::Ptr(new Client()));
	}
	return clients[0]; //note: if clients[0] was deleted, all elements would receive new positions; it is safe to return clients[0] based on vector size.
}