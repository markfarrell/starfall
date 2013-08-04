//
//
//Copyright (c) 2013 Mark Farrell
//
//





//


//









#pragma once

#include "Poco/AutoPtr.h"
#include "Poco/SharedPtr.h"
#include "Poco/Thread.h"
#include "Poco/ThreadPool.h"
#include "Poco/Event.h"
#include "Poco/NObserver.h"

#include "Poco/Net/Net.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/ServerSocket.h"

#include "Poco/Net/SocketNotification.h"
#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketAcceptor.h"


#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"

#include "Poco/JSON/Object.h"

#include "v8.h"

#include <vector>
#include <stack>
#include <map>
#include <string>
#include <iostream>
#include <exception>
#include <sstream>

#include "Starfall/Buffer.h"
#include "Starfall/LoginStruct.h"
#include "Starfall/Head.h"
#include "Starfall/User.h"
#include "Starfall/Player.h"
#include "Starfall/ConsoleStruct.h"
#include "Starfall/Database.h"

using std::vector;
using std::stack;
using std::ostream;
using std::cout;
using std::endl;
using std::string;
using std::exception;

namespace Starfall {

	class LoginConnectionHandler 
	{ 
		private:
				Poco::Net::SocketReactor& _reactor;
				Poco::Net::StreamSocket _socket;
				Player::Ptr _pPlayer;
				Poco::NObserver<LoginConnectionHandler, Poco::Net::ReadableNotification> _readObserver;
				Poco::NObserver<LoginConnectionHandler, Poco::Net::WritableNotification> _writeObserver;
				Poco::NObserver<LoginConnectionHandler, Poco::Net::ShutdownNotification> _shutdownObserver;
				Poco::NObserver<LoginConnectionHandler, Poco::Net::ErrorNotification> _errorObserver;
				Poco::NObserver<LoginConnectionHandler, Poco::Net::TimeoutNotification> _timeoutObserver;

				void end(); 
				/** According to the POCO Documentation this handler must commit suicide (that must mean it uses a pointer behind the scenes):
				 *  "When the ServiceHandler is done, it must destroy itself."
				 */
				void release();
		public:
				LoginConnectionHandler(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor);

				 /**
				  * TODO: Still dequeue the user's send requests in this method after reading.
				  * [Source: Stack Exchange]
				  * http://stackoverflow.com/questions/14632341/poconet-server-client-tcp-connection-event-handler
				  * @Brief Event Handler when Socket becomes Readable, i.e: there is data waiting to be read
				  */
				void onSocketReadable(const Poco::AutoPtr<Poco::Net::ReadableNotification>& pNf);

				/**
				 * [Source: Stack Exchange] ...
				 * @Brief Event Handler when Socket was written, i.e: confirmation of data sent away (not received by client)
				 */
				void onSocketWritable(const Poco::AutoPtr<Poco::Net::WritableNotification>& pNf);

				/**
				 * [Source: Stack Exchange] ...
				 * @Brief Event Handler when Socket was shutdown on the remote/peer side
				 */
				void onSocketShutdown(const Poco::AutoPtr<Poco::Net::ShutdownNotification>& pNf);

				/**
				 * [Source: Stack Exchange] ...
				 * @Brief Event Handler when Socket throws an error
				 */
				void onSocketError(const Poco::AutoPtr<Poco::Net::ErrorNotification>& pNf);

				/**
				 * [Source: Stack Exchange] ...
				 * @Brief Event Handler when Socket times-out
				 */
				void onSocketTimeout(const Poco::AutoPtr<Poco::Net::TimeoutNotification>& pNf);
	};


	/**
	 * Code Taken from the POCO EchoServer Example.
	 */
	class LoginServer
	{
		public:

				LoginServer();
				~LoginServer();
				Poco::UInt16 port() const;
		private:

				Poco::Net::HTTPServer* _pHTTPServer; //The construct of HTTPServer uses RAII. So, we wait to start the server by using a pointer.

				Poco::Net::ServerSocket _socket;
				Poco::Net::SocketReactor _reactor;
				Poco::Net::SocketAcceptor<LoginConnectionHandler> _acceptor;
				Poco::Thread            _thread; //accept connections in this thread; start the socket reactor here
	};


	typedef bool (*LoginHTTPRequestFunction) (User& user, Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse &resp);
	class LoginHTTPRequest {
		friend class LoginHTTPRequestHandler;
		private:
			static std::map<string, Buffer> Files; //Load static HTML/JS data from files in LoginHTTPRequest::Init(); The key is the URI, the same as LoginHTTPRequest::Map;
			static Buffer LoadFile(string fileName); //Loads WEB_PATH+fileName; Note: Pass a const char array and it will be converted into a string. 
			static bool InsertOperation(bool (*insertFunc) (string& jsonString), Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse &resp);
			static Poco::Net::NameValueCollection ReadNameValueCollection(Poco::Net::HTTPServerRequest& req);
		protected:
			static std::map<string, LoginHTTPRequestFunction> Map;
		public: 
			static void Init();
			static bool File(User& user, Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse &resp); //Send a webpage if it is found.
			static bool Console(User& user, Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse& resp);
			static bool MaxConsoleID(User& user, Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse& resp);
			static bool Login(User& user, Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse &resp);
			static bool NewItemType(User& user, Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse &resp);
			static bool SelectItemTypes(User& user, Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse &resp);
			static bool NewItem(User& user, Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse &resp);
			static bool LogAppearance(User& user, Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse &resp);
			static bool Favicon(User& user, Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse &resp);
	};


	class LoginHTTPRequestHandler : public Poco::Net::HTTPRequestHandler {
	
		private:
			v8::Isolate* httpIsolate; //there can only be one instance of v8 per thread, so a new instance must be launched in the http server's thread.
		public:
			LoginHTTPRequestHandler();
			~LoginHTTPRequestHandler();
			virtual void handleRequest(Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse& resp);

	};

	class LoginHTTPRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory {
		public:
			virtual Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& );
	};

}