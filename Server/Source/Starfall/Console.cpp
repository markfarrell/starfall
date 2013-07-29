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


#include "Starfall/Console.h"

using namespace Starfall;

ConsoleInterface http::out;

Poco::JSON::Object::Ptr Console::GetHexMap(string name, string type, Poco::UInt32 beginIdx, Poco::UInt32 endIdx) {
	Poco::JSON::Object* pObj = new Poco::JSON::Object;
	pObj->set("name", name);
	pObj->set("type", type);
	pObj->set("beginIdx", beginIdx);
	pObj->set("endIdx", endIdx);
	return Poco::JSON::Object::Ptr(pObj); //shared pointer; will automatically delete		
}

string Console::Time() {
	return Poco::DateTimeFormatter::format(Poco::DateTime(), Poco::DateTimeFormat::SORTABLE_FORMAT);
}

void Console::Print(Poco::Dynamic::Var& var) {
	if(!Database::Insert::TryConsole(var)) {
		throw new ConsoleException("Console Exception: Database Failed.");
	}
}

