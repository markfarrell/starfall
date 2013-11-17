//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).

#include "Starfall/User.h"

using namespace Starfall;

User::User(string a) {
	this->address = a;
}

void User::clearAll() {
	this->destroyEntityQueue.clear();
	this->transformEntityQueue.clear();
	this->createEntityQueue.clear();
}

