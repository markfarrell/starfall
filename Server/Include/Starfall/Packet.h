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