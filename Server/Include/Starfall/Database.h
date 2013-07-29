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

#include <vector>
#include <iostream>

#include "Poco/Data/Common.h"
#include "Poco/Data/SQLite/Connector.h"

#include "Poco/Dynamic/Var.h"
#include "Poco/JSON/Object.h"
#include "Starfall/LoginStruct.h"

#include "Starfall/ConsoleStruct.h"


namespace Starfall {

	namespace Database {
		class Connection {
			friend class Select;
			friend class Insert;
			protected:
				static std::vector<Poco::Data::Session> SessionContainer; //Vector avoids ptr usage; though it is not a fixed size. Store only 1 session here
				static Poco::Data::Session Session(); //return the single session.
			public:
				static void Init();
				static void Destroy(); //Note: Never gets called, but that's fine. It unregisters the sqlite connector if needed. Though otherwise, no memory is leaked.
		};
		class Select {
			public:
				/* Description: Pass a reference to a login struct that has identified the username and password.
								It will load their usertype and userid.
					Returns: False if multiple users or no users were found. */ 
				static bool TryLogin(LoginStruct& loginStruct);
				static bool TryConsole(std::ostream& out, Poco::UInt32 nRows, Poco::UInt32 offset=0); //Description: Select the n most recent rows; output to the stream provided.
				static bool TryMaxConsoleID(Poco::UInt32 &refMax); //Description: Set the passed variable to the highest console ID in the table. Returns: false if no max ID is found.
				static bool TryItemTypes(ostream& out); 
		};
		class Insert {
			public:
				static bool TryConsole(Poco::Dynamic::Var& var);
				static bool TryItemType(string& jsonString);
				static bool TryNewItem(string& jsonString);
		};
	}

}

