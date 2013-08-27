//Copyright (c) 2013 Mark Farrell
#pragma once

#include <Poco\SharedPtr.h>

#include <vector>
#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>

namespace Starfall {

	class Cube {
		public:

			bool uploaded;

			typedef Poco::SharedPtr<Cube> Ptr;

			GLuint textureID;
			glm::mat4 matrix;
			std::vector<GLfloat> buffer;
			void load(GLfloat scale);
			void load(glm::vec3 scale, glm::vec4 color=glm::vec4(1.0f,1.0f,1.0f,1.0f));

			Cube() { this->textureID = NULL; uploaded = false; }

			void render();
			void upload(sf::Image& front, sf::Image& back, 	sf::Image& left, sf::Image& right, sf::Image& up, sf::Image& down);

		private:
			void push(float x, float y, float z, float s, float t, float r, glm::vec4& color);
	
	};

	class Skybox {
		public:
			glm::vec3 position;
			Skybox(bool load=true); //TODO: PASS a camera class to the constructor to get information for the far clip plane
			void load();
			void render(sf::RenderWindow& window);
		protected:
		
			Cube cube;
			sf::Image front;
			sf::Image back;
			sf::Image left;
			sf::Image right;
			sf::Image up;
			sf::Image down;
	};

}

