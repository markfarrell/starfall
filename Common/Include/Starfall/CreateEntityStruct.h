//Copyright (c) 2013 Mark Farrell
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