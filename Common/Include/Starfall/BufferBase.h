//Copyright (c) 2013 Mark Farrell
#pragma once

#include <Poco/NumberParser.h>
#include <Poco/NumberFormatter.h>
#include <Poco/Net/Net.h>

#include <exception>
#include <vector>
#include <string>
#include <iostream>

using std::cout;
using std::endl;

using std::vector;
using std::exception;
using std::string;
using std::stringstream;

namespace Starfall {

	class BufferException : public exception {
		private:
			string msg;
		public:
			BufferException(const string m="Buffer Exception: No implementation for this type.") : msg(m) { }
			const char* what() { return msg.c_str(); }
	};


	class Buffer : public std::vector<char> {
		public:
		
			string hex(); //represent the contents as a hex string
			void reset(); //reset the position of the iterator

			void writeUInt32(Poco::UInt32 value);
			void writeString(const string& value);
			void writeFloat(float value);

			Poco::UInt32 readUInt32();
			string		 readString();
			float		 readFloat();

			template<typename T> 
			void operator>> (T& into);

			template<typename T>
			void operator<< (T& out);

			/** 
			 * Description: Create an empty buffer. Initializes the iterator position.
			 */
			Buffer(); 
			/** 
			 * Description: Copy data into the buffer. Also initializes the iterator position.
			 */
			Buffer(int length, char* firstChar, char* lastChar);

		private:

			Buffer::iterator _it;
	};

	template<typename T>
	inline void Buffer::operator>> (T& into) {
		BufferException bufferException;
		cout << "[Buffer::operator>>] " << bufferException.what() << endl;
		throw bufferException;
	}
	
	template<typename T>
	inline void Buffer::operator<< (T& out) {
		BufferException bufferException;
		cout << "[Buffer::operator<<] " << bufferException.what() << endl;
		throw bufferException;
	}

}