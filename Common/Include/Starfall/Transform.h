//Copyright (c) 2013 Mark Farrell
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Starfall {

	class Transform {
		public:
			glm::vec3 position;
			glm::vec4 orientation;
	};

}