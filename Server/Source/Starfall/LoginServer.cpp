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



#include "Starfall/LoginServer.h"
#include "Starfall/Ports.h"
#include "Starfall/Send.h"
#include "Starfall/Receive.h"
#include "Starfall/Defines.h"
#include "Starfall/Console.h"
#include "Starfall/Strings.h"
#include "Starfall/Packet.h"
#include "Starfall/Config.h"


#include "Poco/Thread.h"
#include "Poco/Runnable.h"
#include "Poco/RunnableAdapter.h"
#include "Poco/Timespan.h"
#include "Poco/NumberParser.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/IPAddress.h"
#include "Poco/Data/SQLite/Connector.h"

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace Starfall;

using std::ifstream;
using std::getline;
using std::stringstream;

using Poco::Net::Socket;
using Poco::Net::StreamSocket;
using Poco::Net::SocketAddress;
using Poco::Net::IPAddress;



std::map<string, Buffer> LoginHTTPRequest::Files;
std::map<string, LoginHTTPRequestFunction> LoginHTTPRequest::Map;

LoginConnectionHandler::LoginConnectionHandler(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor) 
: _reactor(reactor), 
  _socket(socket),
  _readObserver(*this, &LoginConnectionHandler::onSocketReadable),
  _writeObserver(*this, &LoginConnectionHandler::onSocketWritable),
  _shutdownObserver(*this, &LoginConnectionHandler::onSocketShutdown),
  _errorObserver(*this, &LoginConnectionHandler::onSocketError),
  _timeoutObserver(*this, &LoginConnectionHandler::onSocketTimeout)
{ 
	
	_reactor.addEventHandler(_socket, _readObserver);
	_reactor.addEventHandler(_socket, _writeObserver);
	_reactor.addEventHandler(_socket, _shutdownObserver);
	_reactor.addEventHandler(_socket, _errorObserver);
	_reactor.addEventHandler(_socket, _timeoutObserver);

	_pPlayer = Player::create(_socket.address().toString());

}

void LoginConnectionHandler::end() { 
	//Remove the player: Note the player ptr should only be stored once in a hash map!
	//The reference count should only be one
	_socket.close();

	_reactor.removeEventHandler(_socket, _readObserver);
	_reactor.removeEventHandler(_socket, _writeObserver);
	_reactor.removeEventHandler(_socket, _shutdownObserver);
	_reactor.removeEventHandler(_socket, _errorObserver);
	_reactor.removeEventHandler(_socket, _timeoutObserver);

	this->release();
}

void LoginConnectionHandler::release() {
	delete this;
}

void LoginConnectionHandler::onSocketReadable(const Poco::AutoPtr<Poco::Net::ReadableNotification>& pNf) {

	try {
	
			char data[Head::size];
			int n = _socket.receiveBytes(data, sizeof(data));

			if( n != sizeof(data) ) {
				if(n == 0) { //socket closed
					cout << "Closing connection." << endl;
					this->end();
				} else {
					std::cerr << "Error: n != sizeof(data),  n= " << n << ", sizeof(data)= " << sizeof(data) << endl;
				}
			} else {

				Buffer buffer(n,&data[0], &data[sizeof(data)]);

				Packet<Head> head;
				buffer >> head;
			
				//erase the header
				buffer.erase(buffer.begin(), buffer.begin() + Head::size);
		
				buffer.resize(head->bodysize);
				int n2 = _socket.receiveBytes(&buffer[0], buffer.size());
				if(n2 != buffer.size()) {
					std::cerr << "Error: n2 != sizeof(buffer.size()),  n2= " << n2 << ", sizeof(buffer.size())= " << sizeof(data) << endl;
				}
				buffer.reset();
					
				try {

					ReceiveFunction receiveFunction = Receive::Map.at(head->opcode);
					_pPlayer->lock(); //the player's shared data can be safely accessed while reading packets using a mutex.
					(*receiveFunction)(_pPlayer, buffer, head); //call the function pointer
					_pPlayer->unlock();

					while(!_pPlayer->sendQueue.empty()) {
						Buffer out = _pPlayer->sendQueue.front();
						int n3 = _socket.sendBytes(&(out)[0], out.size()); 
						_pPlayer->sendQueue.pop(); 
					}
	
				}catch(std::out_of_range& oor) {
					std::cerr << "Out of Range error: " << oor.what() << '\n'; //Taken from cplusplus.com
				}

			}
	} catch (Poco::Exception& exc) {
		cout << "LoginServer: " << exc.displayText() << endl;
		this->end();
	}
}


void LoginConnectionHandler::onSocketWritable(const Poco::AutoPtr<Poco::Net::WritableNotification>& pNf) {

}

		
void LoginConnectionHandler::onSocketShutdown(const Poco::AutoPtr<Poco::Net::ShutdownNotification>& pNf) { 
	this->end();
}


void LoginConnectionHandler::onSocketError(const Poco::AutoPtr<Poco::Net::ErrorNotification>& pNf) { 
		cout << "[" << _pPlayer->address << "]" << "Error." << endl;
}


void LoginConnectionHandler::onSocketTimeout(const Poco::AutoPtr<Poco::Net::TimeoutNotification>& pNf) {
	//TODO: Sockets must be able to be timed out; shutdown is not detected automatically unless two sockets connect from the same address.
	cout << "[" << _pPlayer->address << "]" << "Timeout." << endl;

}


/* Note: IPAddress() creates the "0.0.0.0" address */
LoginServer::LoginServer():
	_socket(SocketAddress(IPAddress(), Ports::ToLoginServerFromClient())),
	_thread("LoginServer"),
	_reactor(),
	_acceptor(_socket, _reactor)
{

	_thread.start(_reactor); //Warning: Reactor Locks the socket in the constructor list! Do not call it outside its running thread!
	_pHTTPServer = new Poco::Net::HTTPServer(new LoginHTTPRequestHandlerFactory, Poco::Net::ServerSocket(Ports::ToLoginServerFromWeb()), new Poco::Net::HTTPServerParams);
	_pHTTPServer->start();
	cout << "Login: " << _socket.address().toString() << " / Web: " << _pHTTPServer->port() <<  endl;
}



LoginServer::~LoginServer() {
	std::cout << "Deleting Login Server." << std::endl;

	_pHTTPServer->stop();
	delete _pHTTPServer;

	_reactor.stop();
	_thread.join();
}


Poco::UInt16 LoginServer::port() const {
	return _socket.address().port();
}



Buffer LoginHTTPRequest::LoadFile(string fileName){
	Buffer buffer;
	//string pageString;
	ifstream pageFile(Defines::Path::Plugins()+fileName, std::ios::binary); //Code from: cplusplus.com
	if(pageFile.is_open()){
		pageFile.seekg(0, std::ios::end);
		std::streamoff len = pageFile.tellg();
		pageFile.seekg(0, std::ios::beg);
 		buffer.resize(buffer.size()+((unsigned int)(len)));
		pageFile.read(&buffer[0], len);
	}else{
		cout << "[Web error]: Could not load page / file: " << fileName << endl;
	}
	pageFile.close();
	return buffer;
}

Poco::Net::NameValueCollection LoginHTTPRequest::ReadNameValueCollection(Poco::Net::HTTPServerRequest& req) {
	Poco::Net::NameValueCollection params; 
	std::streamsize len = req.getContentLength();
	if(len > 0) {
		string str(((unsigned int)(len)), '\0');
		req.stream().read(&str[0],len);
		Strings::ReplaceAll(str, "&", ";"); //Needed for Poco::Net::MessageHeader
		Poco::Net::MessageHeader::splitParameters(str.cbegin(), str.cend(), params); //Break the string str into a map.
	}
	return params;
}

bool LoginHTTPRequest::InsertOperation(bool (*insertFunc) (string& jsonString), Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse &resp) {
	std::streamsize len = req.getContentLength();
	if(len > 0) {
		string jsonString(((unsigned int)(len)), '\0');
		req.stream().read(&jsonString[0],len);
		cout << "[New item type]: " << jsonString << endl;
		if(insertFunc(jsonString)) {
			resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
			resp.setContentType("application/json");
			std::ostream& out = resp.send();
			out << "{}" << endl;
			out.flush();
			return true;
		}
	}
	return false;
}

void LoginHTTPRequest::Init() {
	//Load the contents of the map containing URIs and their corresponding file paths.
	map<string,string> resources = Config::URIs();
	for(map<string,string>::iterator it = resources.begin(); it != resources.end(); it++) { 
		LoginHTTPRequest::Files[it->first] = LoginHTTPRequest::LoadFile(it->second);
	}
	LoginHTTPRequest::Map["/ajax/login"] =  &LoginHTTPRequest::Login;
	LoginHTTPRequest::Map["/ajax/console"] = &LoginHTTPRequest::Console;
	LoginHTTPRequest::Map["/ajax/console/new"] = &LoginHTTPRequest::Console;
	LoginHTTPRequest::Map["/ajax/console/more"] = &LoginHTTPRequest::Console;
	LoginHTTPRequest::Map["/ajax/console/max"] = &LoginHTTPRequest::MaxConsoleID;
	LoginHTTPRequest::Map["/ajax/itemtype/new"] = &LoginHTTPRequest::NewItemType;
	LoginHTTPRequest::Map["/ajax/itemtype/select"] = &LoginHTTPRequest::SelectItemTypes;
	LoginHTTPRequest::Map["/ajax/item/new"] = &LoginHTTPRequest::NewItem;
	LoginHTTPRequest::Map["/ajax/modifiers/appearance"] = &LoginHTTPRequest::LogAppearance;
	LoginHTTPRequest::Map["/favicon.ico"] = &LoginHTTPRequest::Favicon;
}



bool LoginHTTPRequest::File(User& user, Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse &resp) {

	//Here is the list of MIME types: en.wikipedia.org/wiki/Internet_media_type
	if(Strings::EndsWith(req.getURI(), "js")){
		resp.setContentType("text/javascript");
	}else if(Strings::EndsWith(req.getURI(), "css")){
		resp.setContentType("text/css");
	} else if(Strings::EndsWith(req.getURI(), "gif")){
		resp.setContentType("image/gif");
	} else if(Strings::EndsWith(req.getURI(), "png")){
		resp.setContentType("image/png");
	} else { 
		resp.setContentType("text/html");
	}

	try{
		std::ostream& out = resp.send();
		Buffer& buffer =  LoginHTTPRequest::Files.at(req.getURI());
		for(std::size_t i = 0; i < buffer.size(); i++)
		{
			out << buffer[i];
		}
		resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
		out.flush();
		return true;
	}catch(std::out_of_range& oor){
		std::cerr << "Cannot find page: " << req.getURI() << " /" << oor.what() << endl;
	}
	return false;
}

bool LoginHTTPRequest::Console(User& user, Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse &resp) {
	Poco::UInt32 maxID = 0;
	Poco::UInt32 upperID = 0;
	Poco::UInt32 offset = 0; //default offset, before checking data sent
	Poco::UInt32 nRows = 10; //default number of rows to load, unless loadAll=true
	Poco::Net::NameValueCollection params = LoginHTTPRequest::ReadNameValueCollection(req);
	if(params.has("offset")) { //optional input from web
		if(!Poco::NumberParser::tryParseUnsigned(params["offset"], offset)) {
			cout << "[Console]: Parsing offset failed!" << endl;
		}
	}
	if(params.has("upper")) { //upper bound; old max ID
		if(!Poco::NumberParser::tryParseUnsigned(params["upper"], upperID)) {
			cout << "[Console]: Parsing max failed!" << endl;
		}
	}
	Database::Select::TryMaxConsoleID(maxID);
	if(upperID != 0 && maxID != 0) {
		if(Strings::EndsWith(req.getURI(), "new")){ //Load from maxID to upperID
			offset = 0;
			nRows = (maxID-upperID); 
		}else{ //Load after  upperID+offset
			offset += (maxID-upperID); 
		}
	}
	resp.setContentType("application/json");
	std::ostream& out = resp.send();
	if(Database::Select::TryConsole(out, nRows, offset)) {
		resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
		out.flush();
		return true;
	} else {
		resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
		out << "{}"; //send empty message; hides more button
		out.flush();
	}
	return false;
}

bool LoginHTTPRequest::MaxConsoleID(User& user, Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse &resp) {
	Poco::UInt32 maxID = 0;
	resp.setContentType("application/json");
	std::ostream& out = resp.send();
	if(Database::Select::TryMaxConsoleID(maxID)){
		Poco::JSON::Object jsonObj;
		jsonObj.set("max", maxID);
		jsonObj.stringify(out);
		resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
		out.flush();
		return true;
	}
	return false;
}

bool LoginHTTPRequest::Login(User& user, Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse &resp) {
	cout << "Login Request: ";
	//To debug a request header write: req.write(cout);

	Poco::Net::NameValueCollection params = LoginHTTPRequest::ReadNameValueCollection(req); 


	for(Poco::Net::NameValueCollection::ConstIterator it = params.begin(); it != params.end(); it++) {
		cout << "[" << it->first << "]=" + it->second << ";";
	} 
	cout << endl;

	if(params.has("username") && params.has("password")) {
		LoginStruct loginStruct;
		loginStruct.username = params["username"];
		loginStruct.password = params["password"];
		if(Database::Select::TryLogin(loginStruct)) {
			Poco::JSON::Object jsonObject; 
			jsonObject.set("userid", loginStruct.userid);
			jsonObject.set("usertype", loginStruct.usertype);
			jsonObject.set("username", loginStruct.username);
			jsonObject.set("password", loginStruct.password);
			cout << "JSON Reply: ";
			jsonObject.stringify(cout, 0);
			cout << endl;
			resp.setContentType("application/json");
			std::ostream& out = resp.send();
			jsonObject.stringify(out, 0);
			resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
			out.flush();
			return true; //success
		}
	}

	return false;
}

bool LoginHTTPRequest::NewItemType(User& user, Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse &resp) {
	return LoginHTTPRequest::InsertOperation(&Database::Insert::TryItemType, req, resp);
}

bool LoginHTTPRequest::SelectItemTypes(User& user, Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse &resp) {
	resp.setContentType("application/json");
	std::ostream& out = resp.send();
	if(Database::Select::TryItemTypes(out)) {
		resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
		out.flush();
		return true;
	}
	return false;
}

bool LoginHTTPRequest::NewItem(User& user, Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse &resp) {
	std::streamsize len = req.getContentLength();
	if(len > 0) {
		string jsonString(((unsigned int)(len)), '\0');
		req.stream().read(&jsonString[0],len);
		if(Database::Insert::TryNewItem(jsonString)) {
			resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
			resp.setContentType("application/json");
			std::ostream& out = resp.send();
			out << "{}" << endl;
			out.flush();
			return true;
		}
	}
	return false;
}

bool LoginHTTPRequest::LogAppearance(User& user, Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse &resp) {
	Config::Appearance();
	resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
	resp.setContentType("application/json");
	std::ostream& out = resp.send();
	out << "{}" << endl;
	out.flush();
	return true;
}

bool LoginHTTPRequest::Favicon(User& user, Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse &resp) {
	return true;
}

LoginHTTPRequestHandler::LoginHTTPRequestHandler() { 
	this->httpIsolate = v8::Isolate::New();
	this->httpIsolate->Enter();
}

LoginHTTPRequestHandler::~LoginHTTPRequestHandler() {
	this->httpIsolate->Exit();
	this->httpIsolate->Dispose();          
}

/**
 * TODO (April 24th, 2013): Handle different requests.
 * These servers should still load the web application (in html and js) from a file. 
 * There are too many issues with cross domain requestions. Additionally, jQuery ajax can allow us to receive 
 * additional requests without reloading the webpage.
 */
void LoginHTTPRequestHandler::handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp) {
	User user(req.clientAddress().toString());
	try
	{
		(LoginHTTPRequest::Map.at(req.getURI()))(user,req,resp);
	}catch(std::out_of_range& oor) {
		if(!LoginHTTPRequest::File(user,req,resp)){
			std::cerr << "HTTP Request not found: " << req.getURI() << " /" << oor.what() << '\n'; //Taken from cplusplus.com
			resp.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
		}
	}
}


Poco::Net::HTTPRequestHandler* LoginHTTPRequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& ) {
	return new LoginHTTPRequestHandler; 
}







