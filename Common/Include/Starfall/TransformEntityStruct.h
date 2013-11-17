//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).
#pragma once

#include "Starfall/Transform.h"

#include <Poco/Foundation.h>

#include <vector>
using std::vector;

namespace Starfall {

	class TransformStruct : public Transform {
		public:
			Poco::UInt32 action;
			TransformStruct();
	};

	class TransformEntityStruct {
		public:
			vector<TransformStruct> path;
			Poco::UInt32 sessionid;
			TransformEntityStruct();
	};

}
