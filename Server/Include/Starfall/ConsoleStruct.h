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

#include <string>
using namespace std;

#include "Poco/Data/Common.h"
#include "Poco/JSON/Object.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/Dynamic/Struct.h"

namespace Starfall {

	class JSONConsoleStruct {
		private: 
			string stringify(Poco::JSON::Object::Ptr& obj); //Uses ostringstream to convert the json object to a string.
			bool extract(Poco::Dynamic::Struct<string> convertedStruct, string name, Poco::JSON::Object::Ptr& into);

		public:
			JSONConsoleStruct();
			Poco::JSON::Object::Ptr head;
			Poco::JSON::Object::Ptr body;
			Poco::JSON::Object::Ptr foot;
			bool copy(Poco::Dynamic::Var& var);
			Poco::Dynamic::Var zip(); //combine into one JSON object
	};

	class FormattedConsoleStruct {
		public:
			Poco::UInt32 id; //Uses SQL lite's ROW ID
			string message;
			FormattedConsoleStruct();
	};

}

namespace Poco {
namespace Data {
	
	template<>
	class TypeHandler<class Starfall::FormattedConsoleStruct> {

	public:
		static std::size_t size() {
			return 4;
		}

		static void bind(std::size_t pos, const Starfall::FormattedConsoleStruct& obj, AbstractBinder* pBinder) {
			//poco_assert_dbg(pBinder != 0);

				//Call namespace, bind interferes with intellisense
			Poco::Data::TypeHandler<Poco::UInt32>::bind(pos++, obj.id, pBinder);
			Poco::Data::TypeHandler<string>::bind(pos++, obj.message, pBinder); //TODO: Is the specialization for POCO::UInt32 implemented?
		}

		static void prepare(std::size_t pos, const Starfall::FormattedConsoleStruct& obj, AbstractPreparation* pPrepare) {
		//	poco_assert_dbg(pPrepare != 0);
			Poco::Data::TypeHandler<Poco::UInt32>::prepare(pos++, obj.id, pPrepare);
			Poco::Data::TypeHandler<string>::prepare(pos++, obj.message, pPrepare);
		}

		static void extract(std::size_t pos, Starfall::FormattedConsoleStruct& obj, const Starfall::FormattedConsoleStruct& defVal, AbstractExtractor* pExt) {
		//	poco_assert_dbg(pExt != 0);
			Poco::UInt32 id;
			string message;
			Poco::Data::TypeHandler<Poco::UInt32>::extract(pos++, id, defVal.id, pExt);
			Poco::Data::TypeHandler<string>::extract(pos++, message, defVal.message, pExt);
			obj.id = id;
			obj.message = message;
		}
	};

} }

