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

#pragma once


#include "Poco/JSON/Object.h"
#include "v8.h"
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
using std::ostringstream;
using std::copy;
using std::ostream_iterator;

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
			static v8::Handle<v8::Value> IO::Run(string scriptName); //Run a script in an environment using main.js; throws an exception if failed
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