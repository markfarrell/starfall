//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).
#pragma once

#include "Starfall/Transform.h"

#include <Poco/Foundation.h>

#include <string>
using std::string;

namespace Starfall {
	class CreateEntityStruct : public Transform {
		public:
			Poco::UInt32 sessionid;
			Poco::UInt32 mode;
			string displayName;
			CreateEntityStruct();
	};
}