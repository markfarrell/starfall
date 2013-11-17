//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Starfall {

	class Transform {
		public:
			glm::vec3 position;
			glm::quat orientation;
	};

}