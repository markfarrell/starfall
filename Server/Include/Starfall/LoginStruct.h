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


#include "Poco/Data/Common.h"
#include "Poco/Net/Net.h"
#include <string>

using std::string;

namespace Starfall { 
	class LoginStruct {

		public:

			Poco::UInt32 state; //0->Not Logged In, 1->Logging In, 2->Logged In; temporary variable
			Poco::UInt32 userid; //Default value 0; means not logged in.
			Poco::UInt32 usertype; //regular=0; admin=1;
			string username;
			string password;

			LoginStruct();
	};

}

/*
 * Description: This class allows the sql session to use(...) a LoginStruct.
 * 
 * Important notes on SQLite Autoincrement / ROWID (sqlite.org):
 *
 * 1) Every row of every table has an 64-bit signed integer ROWID. 
 *
 * 2) The usual algorithm is to give the newly created row a ROWID that
 * is one larger than the largest ROWID in the table prior to the insert. 
 *
 * 3) The normal ROWID selection algorithm described above will generate
 * monotonically increasing unique ROWIDs as long as you never use the
 * maximum ROWID value and you never delete the entry in the table with
 * the largest ROWID.
 */
namespace Poco {
namespace Data {
	
	template<>
	class TypeHandler<class Starfall::LoginStruct> {

	public:
		static std::size_t size() {
			return 4;
		}

		static void bind(std::size_t pos, const Starfall::LoginStruct& obj, AbstractBinder* pBinder) {
			//poco_assert_dbg(pBinder != 0);

			 //Call namespace, bind interferes with intellisense
			Poco::Data::TypeHandler<Poco::UInt32>::bind(pos++, obj.userid, pBinder); //TODO: Is the specialization for POCO::UInt32 implemented?
			Poco::Data::TypeHandler<Poco::UInt32>::bind(pos++, obj.usertype, pBinder);
			Poco::Data::TypeHandler<string>::bind(pos++, obj.username, pBinder);
			Poco::Data::TypeHandler<string>::bind(pos++, obj.password, pBinder);
		}

		static void prepare(std::size_t pos, const Starfall::LoginStruct& obj, AbstractPreparation* pPrepare) {
		//	poco_assert_dbg(pPrepare != 0);
			Poco::Data::TypeHandler<Poco::UInt32>::prepare(pos++, obj.userid, pPrepare);
			Poco::Data::TypeHandler<Poco::UInt32>::prepare(pos++, obj.usertype, pPrepare);
			Poco::Data::TypeHandler<string>::prepare(pos++, obj.username, pPrepare);
			Poco::Data::TypeHandler<string>::prepare(pos++, obj.password, pPrepare);
		}

		static void extract(std::size_t pos, Starfall::LoginStruct& obj, const Starfall::LoginStruct& defVal, AbstractExtractor* pExt) {
		//	poco_assert_dbg(pExt != 0);
			Poco::UInt32 userid;
			Poco::UInt32 usertype;
			string username;
			string password;
			Poco::Data::TypeHandler<Poco::UInt32>::extract(pos++, userid, defVal.userid, pExt);
			Poco::Data::TypeHandler<Poco::UInt32>::extract(pos++, usertype, defVal.usertype, pExt);
			Poco::Data::TypeHandler<string>::extract(pos++, username, defVal.username, pExt);
			Poco::Data::TypeHandler<string>::extract(pos++, password, defVal.password, pExt);
			obj.userid = userid;
			obj.usertype = usertype;
			obj.username = username;
			obj.password = password;
		}
	};

} }