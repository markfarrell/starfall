//Copyright (c) 2013 Mark Farrell
#pragma once

#include <SFML\Graphics.hpp>

namespace Starfall {

	class Camera {

		public:
		sf::Vector3f position;
		sf::Vector3f rotation;

		void initialize(sf::RenderWindow& window);

	};

}