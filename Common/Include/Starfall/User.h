//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).

#pragma once

#include "Starfall/DestroyEntityStruct.h"
#include "Starfall/CreateEntityStruct.h"
#include "Starfall/TransformEntityStruct.h"
#include "Starfall/Buffer.h"

#include <queue>
#include <vector>
#include <string>

using std::string;

namespace Starfall {

	class User {
		public:
			string address;
			std::queue<Buffer> sendQueue;
			std::vector<DestroyEntityStruct> destroyEntityQueue;
			std::vector<CreateEntityStruct> createEntityQueue;
			std::vector<TransformEntityStruct> transformEntityQueue;

			User(string a);
			void clearAll();
	};

}
