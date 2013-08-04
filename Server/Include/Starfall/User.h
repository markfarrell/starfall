//
//
//Copyright (c) 2013 Mark Farrell
//
//





//


//









#pragma once

#include "Poco/Net/Net.h"
#include <string>
#include <queue>
#include <vector>


#include "Starfall/Entity.h"
#include "Starfall/Buffer.h"



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
