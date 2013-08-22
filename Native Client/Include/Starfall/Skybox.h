//Copyright (c) 2013 Mark Farrell
#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>

namespace Starfall {

	class Cube {
		public:
			std::vector<GLfloat> buffer;
			void load(GLfloat scale);
		private:
			void push(float x, float y, float z, float s, float t, float r);
	};

	class Skybox {
		public:
			glm::vec3 position;
			Skybox(bool load=true); //TODO: PASS a camera class to the constructor to get information for the far clip plane
			void load();
			void render(sf::RenderWindow& window);
		protected:
			GLuint textureID;
			Cube cube;
			sf::Image front;
			sf::Image back;
			sf::Image left;
			sf::Image right;
			sf::Image up;
			sf::Image down;
	};

}

