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


#include "Starfall/ConsoleStruct.h"
#include "Starfall/Console.h"
#include <iostream>
#include <sstream>

using namespace Starfall;

using std::cout;
using std::endl;


JSONConsoleStruct::JSONConsoleStruct() {
	this->head = Poco::JSON::Object::Ptr(new Poco::JSON::Object);
	this->body = Poco::JSON::Object::Ptr(new Poco::JSON::Object);
	this->foot = Poco::JSON::Object::Ptr(new Poco::JSON::Object);
}



string JSONConsoleStruct::stringify(Poco::JSON::Object::Ptr& obj) {
	std::ostringstream out; 
	obj->stringify(out,0);
	return out.str();
}

bool JSONConsoleStruct::extract(Poco::Dynamic::Struct<string> convertedStruct, string name, Poco::JSON::Object::Ptr& into) {
	if(convertedStruct.contains(name)) {
		Poco::Dynamic::Var nameVar = convertedStruct[name];
			if(nameVar.isStruct()) {
				Poco::Dynamic::Struct<string> nameStruct = nameVar.extract<Poco::Dynamic::Struct<string>>(); 
				std::set<string> keys = nameStruct.members();
				for(std::set<string>::iterator it = keys.begin(); it != keys.end(); it++) {
					string key = (*it);
					if(nameStruct.contains(key)) {
						into->set(key, nameStruct[key]); 
					}
				}
				return true;
			}
	}
	return false;
}

bool JSONConsoleStruct::copy(Poco::Dynamic::Var& var) {
	if(var.isStruct()) {
		Poco::Dynamic::Struct<string> s = var.extract<Poco::Dynamic::Struct<string>>();
		if(!this->extract(s, "head", this->head)) { return false; }
		if(!this->extract(s, "body", this->body)) { return false; }	
		if(!this->extract(s, "foot", this->foot)) { return false; }
	}
	return true;
}


Poco::Dynamic::Var JSONConsoleStruct::zip() {
	this->head->set("datetime", Console::Time()); //Add extra JSON node for formatted time string
	Poco::JSON::Object::Ptr zippedObj = Poco::JSON::Object::Ptr(new Poco::JSON::Object);
	zippedObj->set("head", this->head);
	zippedObj->set("body", this->body);
	zippedObj->set("foot", this->foot);
	string zippedString = this->stringify(zippedObj);
	return Poco::Dynamic::Var(zippedObj);
}



FormattedConsoleStruct::FormattedConsoleStruct() {
	this->id = 0;
	this->message = "";
}