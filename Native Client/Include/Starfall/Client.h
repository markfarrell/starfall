//Copyright (c) 2013 Mark Farrell
#pragma once

#include "Starfall/User.h"
#include "Starfall/LoginStruct.h"
#include "Starfall/Player.h"


#include <SFML/System/Utf.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>

#include <Poco/Foundation.h>
#include <Poco/SharedPtr.h>
#include <Poco/Thread.h>
#include <Poco/Mutex.h>


#include <vector>
#include <map>

namespace Starfall {


	class Client : private Poco::Runnable {
		friend class ClientSender;
		friend class ClientReceiver;
		friend class LoginControls;
		friend class WorldScene;

		private:
			typedef Poco::SharedPtr<Client> Ptr; //allocate space for clients on the heap so many can be instantiated for benchmarking

			Poco::UInt32 id;

			Poco::Thread thread; //thread separating the network traffic from the rest of the application.
			Poco::Mutex mutex; //a mutex to lock this socket that constantly sends and receives in another thread.
			sf::TcpSocket socket;
			
			Player::Ptr pPlayer;

			bool isConnected; //current connection state of the socket.
			
			
			Client(Poco::UInt32 id);
			virtual void run();
			bool connect(); //trys to connect to the server and runs the thread if successful to send and receive packets
			bool disconnect(); //stops the execution of the thread.

			static Poco::Mutex ClientsMutex; //avoid race condition on accessing Clients in separate threads
			static std::vector<Client::Ptr> Clients; //all clients will be deleted when the program ends automatically.
		public:

			static ClientSender Send;
			static ClientReceiver Receive;

			static Client::Ptr Get(); //get the main client for usage by the application, clients[0]
			static void Clear(); //delete all clients before data in other static structures, such as the collection of Entity IDs, is deleted.

			void setLoginStruct(LoginStruct loginStruct);
			bool tryLogin(LoginStruct loginStruct);
			bool isLoggedIn(); //used by the login scene for a scene transition.


			~Client();

	};

	
	class ClientSender { //ClientSender could be shared between multiple clients; it could be accessed from multiple threads
		friend class Client;
		public:
			typedef bool (*SendFunction) (Player::Ptr& pPlayer);
			Poco::UInt32 at(SendFunction caller); //thread-safe way to get 
			static bool LoginData(Player::Ptr& pPlayer);
			static bool ObjectsData(Player::Ptr& pPlayer);
		private:
			Poco::Mutex mutex;
			std::map<SendFunction, Poco::UInt32> map;
			bool enqueue(SendFunction caller, Buffer& bodyBuffer, Player::Ptr&pPlayer);
			ClientSender();

	};



	class ClientReceiver {
		friend class Client;
		public:
			typedef bool (*ReceiveFunction) (Buffer& buffer, Packet<Head>& head, Player::Ptr& pPlayer);
			ReceiveFunction at(Poco::UInt32 opcode);
		private:
			Poco::Mutex mutex;
			std::map<Poco::UInt32, ReceiveFunction> map;
			ClientReceiver();
			static bool LoginReply(Buffer& buffer, Packet<Head>& head, Player::Ptr& pPlayer);
			static bool ObjectsReply(Buffer& buffer, Packet<Head>& head, Player::Ptr& pPlayer);
			static bool TransformEntityData(Buffer& buffer, Packet<Head>& head, Player::Ptr& pPlayer);
	};
}