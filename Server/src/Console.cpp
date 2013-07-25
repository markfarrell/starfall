#include "Console.h"

namespace http {
	const ConsoleInterface out;
}

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

