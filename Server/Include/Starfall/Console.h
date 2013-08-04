//Copyright (c) 2013 Mark Farrell



#pragma once

#include "Starfall/ConsoleStruct.h"
#include "Starfall/Head.h"
#include "Starfall/Buffer.h"
#include "Starfall/Database.h"

#include "Poco/Data/Common.h"
#include "Poco/JSON/Object.h"
#include "Poco/Dynamic/Var.h"

#include <string>
#include <vector>
#include <exception>

using std::string;
using std::vector;
using std::exception;

namespace Starfall {

	class ConsoleException : public exception { 
		private:
			string msg;
		public:
			ConsoleException(const string m="Console Exception: No implementation for this type.") : msg(m) { }
			const char* what() { return msg.c_str(); }
	};

	class Console {
		private:
			static Poco::JSON::Object::Ptr GetHexMap(string name, string type, Poco::UInt32 beginIdx, Poco::UInt32 endIdx);
		public:
			static void Print(Poco::Dynamic::Var& var); //Data is already formatted as JSON
			template <typename T>
			static void Write(T& data);
			static string Time();
	};

	template<typename T>
	inline void Console::Write(T& data) {
		throw new ConsoleException();
	}

	template<> 
	inline void Console::Write<string>(string& str) {
		Poco::Dynamic::Var message(str);
		if(!Database::Insert::TryConsole(message)) { //a string should be able to be a poco dynamic var
			throw new ConsoleException("Console Exception: Database Failed.");
		}
	}

	template<>
	inline void Console::Write<Packet<Head>>(Packet<Head>& head) {
		JSONConsoleStruct json;
		json.head->set("name", "Head");
		json.body->set("begin", (Poco::UInt32)head->begin);
		json.body->set("opcode", (Poco::UInt32)head->opcode);
		json.body->set("bodysize", (Poco::UInt32)head->bodysize);
		json.body->set("end", (Poco::UInt32)head->end);

		vector<Poco::JSON::Object::Ptr> hexMapVector; //Collection of SharedPtrs; they will be delete once this method returns.
		hexMapVector.push_back(Console::GetHexMap("begin", "UInt32", 0, 3));
		hexMapVector.push_back(Console::GetHexMap("opcode", "UInt32", 4, 7));
		hexMapVector.push_back(Console::GetHexMap("bodysize", "UInt32", 8, 11));
		hexMapVector.push_back(Console::GetHexMap("end", "UInt32", 12, 15));

		json.foot->set("hex", head.getHex());
		json.foot->set("maps", hexMapVector);

		if(!Database::Insert::TryConsole(json.zip())) {
			throw new ConsoleException("Console Exception: Database Failed.");
		}
	}

	template<>
	inline void Console::Write<Packet<LoginStruct>>(Packet<LoginStruct>& loginPacket) {
		JSONConsoleStruct json;
		json.head->set("name", "LoginStruct");
		json.body->set("state", loginPacket->state);
		json.body->set("userid", loginPacket->userid);
		json.body->set("usertype", loginPacket->usertype);
		json.body->set("username", loginPacket->username);
		json.body->set("password", loginPacket->password);

		vector<Poco::JSON::Object::Ptr> hexMapVector; //Collection of SharedPtrs; they will be delete once this method returns.

		Poco::UInt32 begin = 0;
		Poco::UInt32 end = 3;

		hexMapVector.push_back(Console::GetHexMap("state", "UInt32", begin, end)); 

		begin = end + 1;
		end = end + 4;

		hexMapVector.push_back(Console::GetHexMap("userid", "UInt32", begin, end)); 

		begin = end + 1;
		end = end + 4;

		hexMapVector.push_back(Console::GetHexMap("usertype", "UInt32", begin, end));

		begin = end + 1; 
		end = end + 4 + loginPacket->username.size();

		hexMapVector.push_back(Console::GetHexMap("username", "String", begin, end));

		begin = end + 1;
		end = end + 4 + loginPacket->password.size();

		hexMapVector.push_back(Console::GetHexMap("password", "String", begin, end));


		//TODO: This code should be a separate function
		json.foot->set("hex", loginPacket.getHex());
		json.foot->set("maps", hexMapVector);

		if(!Database::Insert::TryConsole(json.zip())) {
			throw new ConsoleException("Console Exception: Database Failed.");
		}
	}


	/** 
	 * Description: Adds syntactic sugar to console outputs.
	 * E.g. http::out << obj1 << obj2;
	 */
	class ConsoleInterface {
		public:
			template<typename T>
			const ConsoleInterface& operator<< (T& data) const;

	};

	template<typename T>
	inline const ConsoleInterface& ConsoleInterface::operator<< (T& data) const {
		Console::Write(data);
		return (*this);
	}


	class http {
		public:
			static ConsoleInterface out;
	};

}