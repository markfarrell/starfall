//Copyright (c) 2013 Mark Farrell

#include "Starfall\Isolates.h"

using namespace Starfall;

Poco::Mutex  Isolates::Mutex;
map<v8::Isolate*, Poco::UInt32> Isolates::Map;


v8::Isolate* Isolates::Acquire() {

	v8::Isolate* currentIsolate = v8::Isolate::GetCurrent(); //isolate per thread
	if(currentIsolate == NULL) {
		currentIsolate = v8::Isolate::New();
		currentIsolate->Enter(); //enter the isolate when created
	}

	Isolates::Mutex.lock();
	if(Isolates::Map.find(currentIsolate) != Isolates::Map.end()) {
		Isolates::Map[currentIsolate] = Isolates::Map[currentIsolate] + 1;
	} else {
		Isolates::Map[currentIsolate] = 1;
	}
	Isolates::Mutex.unlock();

	return currentIsolate;
}

Poco::UInt32 Isolates::Release(v8::Isolate* releaseIsolate) {
	Isolates::Mutex.lock();
	if(Isolates::Map.find(releaseIsolate) != Isolates::Map.end()) {
		Isolates::Map[releaseIsolate] = Isolates::Map[releaseIsolate] - 1;
		if(Isolates::Map[releaseIsolate] == 0) {
			releaseIsolate->Exit();
			releaseIsolate->Dispose();
			Isolates::Map.erase(releaseIsolate);
		} else {
			return Isolates::Map[releaseIsolate];
		}
	}
	Isolates::Mutex.unlock();
	return 0;
}