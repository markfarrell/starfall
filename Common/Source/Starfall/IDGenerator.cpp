//Copyright (c) 2013 Mark Farrell
#include "Starfall\IDGenerator.h"

using namespace Starfall;

Poco::UInt32 IDGenerator::id = 0;
Poco::Mutex  IDGenerator::mutex;

Poco::UInt32 IDGenerator::next() {	
	Poco::UInt32 nextID = 0;
	IDGenerator::mutex.lock();
	IDGenerator::id++;
	nextID = IDGenerator::id;
	IDGenerator::mutex.unlock();
	return nextID;
}