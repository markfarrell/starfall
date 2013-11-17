//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).
#pragma once

#include <Poco/Foundation.h>
#include <Poco/Mutex.h>

#include <v8.h>

#include <map>
using std::map;

namespace Starfall {
	class Isolates {
		friend class Entity;
		protected:
			static v8::Isolate* Acquire(); //Create a new isolate if one doesn't exist in the current thread. Stores the isolate pointer in a map and increases its reference count.
			/** 
				Description:
					Decrease the reference count on a given isolate shared by entities in the same thread.
				Returns: 
					The new count.
					If the count is 0, the isolate has been exited from and disposed. 
				*/
			static Poco::UInt32 Release(v8::Isolate* releaseIsolate); 
		private:
			static Poco::Mutex Mutex; 
			static map<v8::Isolate*, Poco::UInt32> Map; //Key: Isolate pointer; Value: Reference Count

	};
}
