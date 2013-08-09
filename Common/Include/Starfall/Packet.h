//Copyright (c) 2013 Mark Farrell

#pragma once

#include "Poco/SharedPtr.h"

#include <string>
using std::string;

namespace Starfall {

	template<typename T>
	class Packet {
		private: 

			Poco::SharedPtr<T> pData; //memory safe
			string hex;
		public:

			Packet() { 
				this->pData.assign((new T));
			}

			/* 
			 * Description: Access pData directly: E.g. (packet)->foo() results in a call to (data)->foo.
			 * The compiler will expand calls to this operator.
			 */
			T* operator->() {
				return this->pData.get();
			}
		

			void setHex(string& hex) { 
				this->hex = hex;
			}

			string& getHex() {
				return this->hex;
			}

			T& value() { 
				return ( *( this->pData.get() ) );
			}

	};

}