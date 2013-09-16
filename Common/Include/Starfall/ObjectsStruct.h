//Copyright (c) 2013 Mark Farrell
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