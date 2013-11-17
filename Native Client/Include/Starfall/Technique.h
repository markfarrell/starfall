//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).
#pragma once

#include "Starfall/Assets.h"

#include <vector>
#include <Poco/SharedPtr.h>

using std::vector;

namespace Starfall {

	class Pass {
		public:
			typedef Poco::SharedPtr<Pass> Ptr;
			virtual void beginPass();
			virtual void endPass();
	};

	class Technique : public Asset {
		friend class Assets;
		friend class Model;
		protected:
			Technique();
			vector<Pass::Ptr> passes;
			virtual void beginPasses();
			virtual void endPasses();
		public:
			typedef Poco::SharedPtr<Technique> Ptr;
			virtual void load();
	};
}