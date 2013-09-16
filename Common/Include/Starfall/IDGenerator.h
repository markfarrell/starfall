//Copyright (c) 2013 Mark Farrell
#pragma once

#include <Poco/Foundation.h>
#include <Poco/Mutex.h>

#include <exception>
#include <string>

using std::string; 
using std::exception;

namespace Starfall { 
	class IDException : public exception { 
		private:
			string msg;
		public:
			IDException(const string m="IDException: The maximum possible ID has been exceeded.") : msg(m) { }
			const char* what() { return msg.c_str(); }
			virtual ~IDException() throw() {}
	};

	class IDGenerator {
		friend class Entity;
		friend class Player;
		private:
			static Poco::Mutex mutex; 
			static Poco::UInt32 id;
		protected:
			static Poco::UInt32	Next(); //uses a mutex to stop simultaneous access
	};
}