#include "User.h"
#include "Entity.h"

using namespace Starfall;

User::User(string a) {
	this->address = a;
}

void User::clearAll() {
	this->destroyEntityQueue.clear();
	this->transformEntityQueue.clear();
	this->createEntityQueue.clear();
}

