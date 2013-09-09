//Copyright (c) 2013 Mark Farrell
#pragma once


#include <Poco/JSON/Object.h>
#include <v8.h>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>


using std::ifstream;
using std::map;
using std::string;
using std::cout;
using std::wcout;
using std::endl;
using std::copy;
using std::ostringstream;

namespace Starfall {

	class IO {
		friend class JS;
		friend class Config;
		friend class Entity;

		private:
			static map<string,string> Resources;
		protected:
			static string LoadSource(string fileName);
			static v8::Handle<v8::String> LoadScript(string scriptName);
			static bool TryScript(string scriptName, v8::Handle<v8::Value>& result); //try to execute one script, given an execution environment exists; use run
			static v8::Persistent<v8::Context> Context(); //Instantiate a script environment
			static v8::Handle<v8::Value> Run(string scriptName); //Run a script in an environment using main.js; throws an exception if failed
			static string Resource(string fileName);
	};

	/**
	 * Description: 
	 * A class to provide wrappers for global functions in a V8 context.
	 */
	class JS {
		friend class IO;
		protected:
			static v8::Handle<v8::Value> Include(const v8::Arguments& args); //javascript "import wrapper"
			static v8::Handle<v8::Value> Print(const v8::Arguments& args);
			static v8::Handle<v8::Value> Data(const v8::Arguments& args); 
			static v8::Handle<v8::Value> Http(const v8::Arguments& args); 
			static v8::Handle<v8::Value> Time(const v8::Arguments& args); 
	};

	/**
	 * Description: A global way of interfacing with server configurations.
	 * Methods generally return JSON objects. These could be returned from script,
	 * or manually generating in C++. The accessor of these methods wouldn't 
	 * know the difference.
	 */
	class Config {
		public:
			static Poco::JSON::Object::Ptr PlayerPool(); //safe ptr
			static map<string,string> URIs();
			static string Appearance();
	};

}
