//Copyright (c) 2013 Mark Farrell


#include "Starfall/Config.h"

#include "Starfall/Console.h"
#include "Starfall/Defines.h"
#include "Starfall/Buffer.h"

using namespace Starfall;

map<string,string> IO::Resources;

v8::Handle<v8::Value> JS::Include(const v8::Arguments& args) {
	v8::HandleScope handleScope(args.GetIsolate());
	for (int i = 0; i < args.Length(); i++) {
		if(args[i]->IsString()) {
			string scriptName = *v8::String::AsciiValue(args[i]->ToString());
			v8::Handle<v8::Value> result;
			if(IO::TryScript(scriptName, result)) {
				return v8::Boolean::New(true);
			}
		}
	}
	return v8::Boolean::New(false);
}

v8::Handle<v8::Value> JS::Print(const v8::Arguments& args) {
	v8::HandleScope handleScope(args.GetIsolate());
	for (int i = 0; i < args.Length(); i++) {
		if(args[i]->IsString()) {
			string message = *v8::String::AsciiValue(args[i]->ToString());
			cout << "[JS]:"  << message << endl;
		}
	}
	return v8::Boolean::New(false);
}

v8::Handle<v8::Value> JS::Data(const v8::Arguments& args) {
	if(args.Length() == 1) {
		return v8::String::New(IO::Resource(Defines::Path::Data()+*v8::String::AsciiValue(args[0]->ToString())).c_str());
	}
	return v8::Undefined();
}

v8::Handle<v8::Value> JS::Http(const v8::Arguments& args) {
	if(args.Length() == 1) {
		string message = *v8::String::AsciiValue(args[0]->ToString());
		http::out << message;
	}
	return v8::Undefined();
}

v8::Handle<v8::Value> JS::Time(const v8::Arguments& args) {
	return v8::String::New(Console::Time().c_str());
}

string IO::LoadSource(string fileName) { 
	Buffer buffer;
	ifstream pageFile(fileName.c_str(), std::ios::binary); //Code from: cplusplus.com
	if(pageFile.is_open()){
		pageFile.seekg(0, std::ios::end);
		std::streamoff len = pageFile.tellg();
		pageFile.seekg(0, std::ios::beg);
 		buffer.resize(buffer.size()+((unsigned int)(len)));
		if(buffer.size() > 0) { 
			pageFile.read(&buffer[0], len);
		}
	}else{
		throw Poco::FileNotFoundException("[Load source error]: Could not find file");
	}
	pageFile.close();
	ostringstream ss;
	std::copy(buffer.begin(), buffer.end(), std::ostream_iterator<char>(ss));
	return ss.str();
}

v8::Handle<v8::String> IO::LoadScript(string scriptName) {
	string source = IO::LoadSource(Defines::Path::Plugins()+scriptName);
	return v8::String::New(source.c_str(), source.length());
}

bool IO::TryScript(string scriptName, v8::Handle<v8::Value> &result) {
	v8::TryCatch tryCatch;
	v8::Handle<v8::String> sourceHandle;
	try {
		sourceHandle = IO::LoadScript(scriptName);
	} catch (Poco::FileNotFoundException e) {
		cout << e.message() << ":" << scriptName << endl;
		return false;
	}
	if(!sourceHandle.IsEmpty()) {
		v8::Handle<v8::Script> scriptHandle = v8::Script::Compile(sourceHandle, v8::String::New(scriptName.c_str()));
		if(scriptHandle.IsEmpty()) { //report exceptions
			v8::Handle<v8::Message> message = tryCatch.Message();
			if(message.IsEmpty()) {
				cout << "V8 Exception: " << *v8::String::AsciiValue(tryCatch.Exception()) << endl;
			} else { //print stack trace
				int linenum = message->GetLineNumber();
				int start = message->GetStartColumn();
				int end = message->GetEndColumn();
				cout << *v8::String::AsciiValue(message->GetScriptResourceName()) << ":" << linenum << " " << *v8::String::AsciiValue(tryCatch.Exception()) << endl;
				cout << *v8::String::AsciiValue(message->GetSourceLine()) << endl;
				for(int i = 0; i < start; i++) {
					cout << " ";
				}
				for(int i = 0; i < end; i++) {
					cout << "^";
				}
				cout << endl;
				cout << *v8::String::AsciiValue(tryCatch.StackTrace());
				cout << endl;
			}
		} else {
			result = scriptHandle->Run();
			return true;
		}
	}
	return false;
}

v8::Persistent<v8::Context>  IO::Context() {
	v8::HandleScope handleScope(v8::Isolate::GetCurrent());
	v8::Handle<v8::ObjectTemplate> globalTemplate = v8::ObjectTemplate::New();
	globalTemplate->Set(v8::String::New("Include"), v8::FunctionTemplate::New(JS::Include));
	globalTemplate->Set(v8::String::New("Print"), v8::FunctionTemplate::New(JS::Print));
	globalTemplate->Set(v8::String::New("Data"), v8::FunctionTemplate::New(JS::Data));
	globalTemplate->Set(v8::String::New("Http"), v8::FunctionTemplate::New(JS::Http));
	globalTemplate->Set(v8::String::New("Time"), v8::FunctionTemplate::New(JS::Time));
	return v8::Context::New(NULL, globalTemplate);
}

v8::Handle<v8::Value> IO::Run(string scriptName) {
	bool fail = false;
	v8::Handle<v8::Value> result;
	if(IO::TryScript(scriptName, result)) {
		if(IO::TryScript("Script/main.js", result)) {
			fail = false;
		} 
	}
	
	if(fail) {
		throw Poco::RuntimeException("This script has failed to run.");
	}
	return result;
}

string IO::Resource(string fileName) {
	if(IO::Resources.find(fileName) == IO::Resources.end()) {
		return IO::LoadSource(fileName);
	}
	return IO::Resources[fileName];
}

string Config::Appearance() {

	v8::HandleScope handleScope(v8::Isolate::GetCurrent());
	v8::Persistent<v8::Context> persistentContext = IO::Context();
	v8::Context::Scope	persistentContextScope(persistentContext); //enter global scope first
	v8::Handle<v8::Value> result = IO::Run("Script/Entity/appearance.js");
	if(!result.IsEmpty()) {
			
	} 
	persistentContext.Dispose(); //delete the persistent handle; must be done manually.
	return IO::Resource(Defines::Path::Data()+"Entity/appearance.xml");
}

Poco::JSON::Object::Ptr Config::PlayerPool() { 
	Poco::JSON::Object::Ptr pObj(new Poco::JSON::Object);
	pObj->set("name", "Player Pool");
	pObj->set("minCapacity", ((Poco::UInt32) 1));
	pObj->set("maxCapacity", ((Poco::UInt32) 200));
	pObj->set("idleTime", ((Poco::UInt32) 60));
	pObj->set("stackSize", 0);
	return pObj;
}

map<string,string> Config::URIs() {
	map<string,string> uriMap;
	try {
		v8::HandleScope handleScope(v8::Isolate::GetCurrent());
		v8::Persistent<v8::Context> persistentContext = IO::Context();
		v8::Context::Scope	persistentContextScope(persistentContext); //enter global scope first
		v8::Handle<v8::Value> result = IO::Run("Script/HTTP/resources.js");
		if(!result.IsEmpty()) {
			if(result->IsObject()) {
				v8::Handle<v8::Array> keys = result->ToObject()->GetPropertyNames();
					int length = keys->Length();
					for(int i = 0; i < length; i++) {
						v8::Local<v8::Value> element = keys->Get(i); 
						string uri = *v8::String::AsciiValue(element->ToString());
						string file = *v8::String::AsciiValue(result->ToObject()->Get(element)->ToString());
						uriMap[uri] = file;
					}
			}
		} 
		persistentContext.Dispose(); //delete the persistent handle; must be done manually.
	} catch(Poco::RuntimeException e) {
		cout << "[Config::URIs]:" << e.message() << endl;
	}
	return uriMap;
}
