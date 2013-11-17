//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).
#pragma once

#include <Poco/ScopedLock.h>
#include <Poco/Mutex.h>
#include <SFML/Graphics.hpp>

#include <map>
#include <string>

using std::map;
using std::string;

namespace Starfall {
	class Images {
		private:
			static Poco::Mutex ResourcesMutex;
			static map<string, sf::Image> Resources;
		public:
			static bool Load(sf::Image& value, string path); //load the file into the value
	};

}