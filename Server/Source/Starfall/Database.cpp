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


#include "Poco/SharedPtr.h"

#include "Poco/DateTime.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/DateTimeFormatter.h"

#include "Starfall/Database.h"
#include "Starfall/Strings.h"
#include "Starfall/Defines.h"

#include <iostream>
#include <sstream>
#include <fstream>

using namespace Starfall;

using std::cout;
using std::endl;


std::vector<Poco::Data::Session> Database::Connection::SessionContainer;


Poco::Data::Session Database::Connection::Session() { 
	if(Database::Connection::SessionContainer.size() == 0) {
		Database::Connection::Init();
	}
	return Database::Connection::SessionContainer[0];
}

void Database::Connection::Init() { 
	Poco::Data::SQLite::Connector::registerConnector();
	Database::Connection::SessionContainer.push_back(Poco::Data::Session("SQLite", Defines::Path::Data()+"server.db"));
}

void Database::Connection::Destroy() {
	Database::Connection::SessionContainer.clear();
	Poco::Data::SQLite::Connector::unregisterConnector();
}

bool Database::Select::TryLogin(LoginStruct& loginStruct) {
	bool ret = false;

	
	std::vector<LoginStruct> logins;
	string query = "SELECT * FROM LOGIN WHERE USERNAME='"+loginStruct.username+"' AND PASSWORD='"+loginStruct.password+"'";
	Database::Connection::Session() << query, Poco::Data::into(logins), Poco::Data::now;
	if(logins.empty()) {
		cout << "This user does not exist in the database." << endl;
	}
	else if(logins.size() == 1) {
		cout << "(Found this user in the database)" << " userid=" << logins[0].userid << ";" << " usertype=" << logins[0].usertype << endl;
		if(logins[0].userid != loginStruct.userid) {
			loginStruct.userid = logins[0].userid;
			loginStruct.usertype = logins[0].usertype;
			ret = true; //Success
		} else {
			std::cerr << "Error: Database has selected an entry containing an invalid userid("<<logins[0].userid<<")"<< endl;
		}
	}
	else {
		std::cerr << "Error: Database has duplicates of this username and password combination." << endl;
	}
	return ret;
}


bool Database::Select::TryConsole(std::ostream& out, Poco::UInt32 nRows, Poco::UInt32 offset) {
	std::vector<FormattedConsoleStruct> consoles;
	Poco::UInt32 maxID = 0;
	if(!Database::Select::TryMaxConsoleID(maxID)) { //Fail conditions
		return false;
	}
	if(maxID-offset < 0) { //Subtract 1 from nRows since row at the offset is loaded.
		return false;
	}
	ostringstream queryStream;
	queryStream << "SELECT * FROM CONSOLE ORDER BY ID DESC LIMIT " << nRows << " OFFSET " << offset;
	string query = queryStream.str();
	Database::Connection::Session() << query, Poco::Data::into(consoles),  Poco::Data::now; //Poco::Data::limit(nRows)
	Poco::UInt32 minID = 0; //track minID
	if(!consoles.empty()) {
		vector<Poco::Dynamic::Var> messages; //[{head...,body...,foot...}, {head...,body...,foot...}, ...]
		Poco::JSON::Object::Ptr obj = Poco::JSON::Object::Ptr(new Poco::JSON::Object); // {message: [...]}
		for(std::vector<FormattedConsoleStruct>::iterator it = consoles.begin(); it != consoles.end(); it++){

			if(minID > (*it).id || minID == 0) {
				minID = (*it).id;
			}

			messages.push_back(Poco::Dynamic::Var::parse((*it).message));

		}

		Poco::UInt32 offset = (maxID-minID)+1; //future offset for "more" requests

		obj->set("messages", messages);
		obj->set("offset", offset);
		obj->set("upper", maxID);

		obj->stringify(out);

		return true;
	}
	cout << "[TryConsole]: Fail / Console empty." << endl;
	return false;
}

bool Database::Select::TryMaxConsoleID(Poco::UInt32& refMax) {
	refMax = 0;
	Database::Connection::Session() << "SELECT MAX(ID) FROM CONSOLE", Poco::Data::into(refMax), Poco::Data::now;
	if(refMax != 0) {
		return true;
	}
	return false;
}

bool Database::Select::TryItemTypes(ostream& out) { 
	vector<string> itemTypes; 
	Database::Connection::Session() << "SELECT PROPERTIES FROM ITEMTYPES", Poco::Data::into(itemTypes), Poco::Data::now;
	if(itemTypes.size() > 0) {
		//Build { itemtypes: [{...}, {...}, ...] }
		out << "{";
		out << "\"" << "itemTypes" << "\"" << ":";
		out << "[";
				for(std::vector<string>::iterator it = itemTypes.begin(); it != itemTypes.end(); it++) {
					out << (*it);
					if((it+1) != itemTypes.end()) {
						out << ",";
					}
				}
		out << "]";
		out << "}";
		return true;
	}
	return false;
}

bool Database::Insert::TryItemType(string& jsonString) { 
	Database::Connection::Session() << "INSERT INTO ITEMTYPES (properties) VALUES (:valproperty)", Poco::Data::use(jsonString), Poco::Data::now;
	return true;
}

bool Database::Insert::TryNewItem(string& jsonString) { 
	Database::Connection::Session() << "INSERT INTO ITEMS (data) VALUES (:valdata)", Poco::Data::use(jsonString), Poco::Data::now;
	return true;
}


bool Database::Insert::TryConsole(Poco::Dynamic::Var& var) {
	string messageString = "";
	if(var.isString()) { //assume the var is in JSON format
		var.convert(messageString);
	} else { //assume the var needs to be formatted to JSON
		messageString = Poco::Dynamic::Var::toString(var);
		Strings::ReplaceAll(messageString, "\n", ""); //The parser inserts these escape chars. This is invalid JSON and shouldn't be stored in the DB.
		Strings::ReplaceAll(messageString, "\t", ""); //
	}
	Database::Connection::Session() << "INSERT INTO CONSOLE (message) VALUES (:valmessage)", Poco::Data::use(messageString), Poco::Data::now;
	return true;
}

