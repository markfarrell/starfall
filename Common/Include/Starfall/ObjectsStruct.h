//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).
#pragma once

#include <Poco/Foundation.h>

#include <vector>

using std::vector;

namespace Starfall {

	class ObjectsStruct {
		public:

			Poco::UInt32 state;
			float farClipDistance;

			vector<Poco::UInt32> ids;

			ObjectsStruct();
	};

}