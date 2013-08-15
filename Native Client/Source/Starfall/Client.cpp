//Copyright (c) 2013 Mark Farrell
#include "Starfall\Client.h"


#include "Starfall\ConfigurationFile.h"
#include "Starfall\Head.h"
#include "Starfall\Packet.h"
#include "Starfall\Buffer.h"

#include <Poco\NumberParser.h>
#include <Poco\StringTokenizer.h>
#include <Poco\ScopedLock.h>

#include <string>

using std::string;

using namespace Starfall;

ClientSender Client::Send;
ClientReceiver Client::Receive;

std::vector<Client::Ptr> Client::Clients;


Client::Client(Poco::UInt32 v)
	: id(v),
	  isConnected(false),
	  user(ConfigurationFile::Client().getString("server.address")) //User address is set based on INET address format using config
{

} 

Client::~Client() {
	this->disconnect();
	if(this->thread.isRunning()) {
		this->thread.join();
	}
}

void Client::run() {
	while(true) {
		Poco::ScopedLock<Poco::Mutex> lock(this->mutex); //returning to stop the thread before unlocking the mutex can cause a deadlock; scope locked seemed safer.
		if(this->isConnected) {

			//Receive a Packet
			Buffer headBuffer;
			Buffer bodyBuffer;
			Packet<Head> head;
			std::size_t headReceived = 0;
			std::size_t bodyReceived = 0;

			headBuffer.resize(Head::size);
			headBuffer.reset();

			if(this->socket.receive(&headBuffer[0], (std::size_t)(Head::size), headReceived) == sf::Socket::Done) {
				if(headReceived == Head::size) {
					headBuffer >> head;
					if(head->bodysize > 0) {

						bodyBuffer.resize(head->bodysize);
						bodyBuffer.reset();

						if(this->socket.receive(&bodyBuffer[0], bodyBuffer.size(), bodyReceived) == sf::Socket::Done) {
							if(bodyReceived > 0) {
								try {

									 ClientReceiver::ReceiveFunction receiveFunction = Client::Receive.at(head->opcode);

									 for(vector<Client::Ptr>::iterator it = Client::Clients.begin(); it != Client::Clients.end(); it++) { //find the original sharedptr to pass to the receive function; update the reference count properly
										 if((*it)->id == this->id) {
											 (*receiveFunction)(bodyBuffer, head, (*it)); //call the function pointer
										 }
									 }

									
								} catch(std::out_of_range& oor) {
									std::cerr << "Out of Range error: " << oor.what() << '\n'; 
								}
							}
						}
					}
				}
			}
			
			//Send Packets
			while(!this->user.sendQueue.empty()) {
				Buffer buffer = this->user.sendQueue.front();
				if(this->socket.send(&buffer[0], buffer.size()) == sf::Socket::Done) {
					cout << "test";
					this->user.sendQueue.pop(); //delete the buffer if the socket was ready to send the data; else try again
				}
			}

		} else {
			return; //stop the thread if the client disconnects
		}
	}
}

bool Client::connect() {
	Poco::ScopedLock<Poco::Mutex> lock(this->mutex);
	if(!this->isConnected) {
		if(!this->socket.isBlocking()) {
			this->socket.setBlocking(true); //keep blocking state until connected.
		}
		Poco::StringTokenizer tokenizer(this->user.address, ":", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
		if(tokenizer.count() == 2) { //ip address, port
			string ipString = tokenizer[0];
			int port;
			if(Poco::NumberParser::tryParse(tokenizer[1], port)) {
				if(this->socket.connect(sf::IpAddress(ipString), (unsigned short)(port), sf::seconds(3)) != sf::Socket::Error) {
					this->socket.setBlocking(false); //turn off blocking state for sending and receiving
					this->isConnected = true;
					this->thread.start(*this);
					return true;
				}
			}
		} 
		return false;
	}
	return true; //true if already connected.
}

bool Client::disconnect() {
	Poco::ScopedLock<Poco::Mutex> lock(this->mutex);

	if(this->isConnected) {
		this->isConnected = false;
		this->socket.disconnect();
		return true;
	}

	return false;
}

void Client::setLoginStruct(LoginStruct loginStruct) {
	Poco::ScopedLock<Poco::Mutex> lock(this->mutex);
	this->loginStruct = loginStruct;
}

bool Client::tryLogin(LoginStruct loginStruct) {
	Poco::ScopedLock<Poco::Mutex> lock(this->mutex); //mutexes are recursive; this mutex gets locked twice and unlocked twice; this is safe
	if(this->loginStruct.state == LOGIN_STATE_NOT_LOGGED_IN) { //only allow the user to log in once before a result is received
		this->setLoginStruct(loginStruct);
		ClientSender::LoginData();
		return true;
	}
	return false;
}

Client::Ptr Client::Get() {
	if(Client::Clients.size() == 0) {
		Client::Clients.push_back(Client::Ptr(new Client(Client::Clients.size())));
	}
	return Client::Clients[0]; //note: if clients[0] was deleted, all elements would receive new positions; it is safe to return clients[0] based on vector size.
}

ClientSender::ClientSender() {
	this->map[&ClientSender::LoginData] = 0x01;
}

bool ClientSender::enqueue(SendFunction caller, Buffer& bodyBuffer, Client::Ptr& pClient) {

	Buffer buffer;
	Buffer headBuffer;
	Head head;

	head.begin = 0xFFFFFFFF;
	head.opcode = this->map.at(caller);
	head.bodysize = bodyBuffer.size();
	head.end = 0xFFFFFFFF;

	headBuffer << head;

	buffer.insert(buffer.end(), headBuffer.begin(), headBuffer.end());
	buffer.insert(buffer.end(), bodyBuffer.begin(), bodyBuffer.end());

	pClient->mutex.lock(); //queue will be read in another thread.
	pClient->user.sendQueue.push(buffer);
	pClient->mutex.unlock();

	return true;
}

bool ClientSender::LoginData(Client::Ptr& pClient) {
	pClient->mutex.lock(); 
	Buffer buffer;
	buffer << pClient->loginStruct;
	Client::Send.enqueue(&ClientSender::LoginData, buffer, pClient);
	pClient->mutex.unlock();
	return true;
}

Poco::UInt32 ClientSender::at(SendFunction caller) { 
	Poco::UInt32 ret = 0;
	this->mutex.lock();
	ret = this->map.at(caller);
	this->mutex.unlock();
	return ret;
}

ClientReceiver::ClientReceiver() {
	this->map[0x10000000 | 0x01] = &ClientReceiver::LoginReply;
}

ClientReceiver::ReceiveFunction ClientReceiver::at(Poco::UInt32 opcode) {
	ReceiveFunction receiveFunction;
	this->mutex.lock();
	receiveFunction = this->map.at(opcode);
	this->mutex.unlock();
	return receiveFunction;
}

bool ClientReceiver::LoginReply(Buffer& buffer, Packet<Head>& head, Client::Ptr& pClient) {
	Packet<LoginStruct> loginStruct;
	buffer >> loginStruct;
	pClient->setLoginStruct(loginStruct.value());

	return true;
}