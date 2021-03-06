//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).
#pragma once

#include "Starfall/CreateEntityStruct.h"
#include "Starfall/DestroyEntityStruct.h"

#include <Poco/Foundation.h>

#include <vector>
using namespace std;

namespace Starfall {

	class ObjectsUpdateStruct { 
		public:

			Poco::UInt32 state;
			float farClipDistance;

			vector<CreateEntityStruct> createEntities;
			vector<DestroyEntityStruct> destroyEntities;

			ObjectsUpdateStruct();
	};
}

