//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).

#include "Starfall/LoginStruct.h"

using namespace Starfall;

LoginStruct::LoginStruct() {
	this->state = 0;
	this->userid = 0;
	this->usertype = 0;
	this->username = "";
	this->password = "";
}
