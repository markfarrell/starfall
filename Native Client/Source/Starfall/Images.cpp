//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).
#include "Starfall/Images.h"

using namespace Starfall;

Poco::Mutex Images::ResourcesMutex;
map<string, sf::Image> Images::Resources;

bool Images::Load(sf::Image& value, string path) {
	Poco::ScopedLock<Poco::Mutex> lock(Images::ResourcesMutex);

	if(Images::Resources.find(path) != Images::Resources.end()) {
		value = Images::Resources[path];
		return true;
	} else {
		bool ret = value.loadFromFile(path);
		Images::Resources[path] = value;
		return ret;
	}
	return false;
}
