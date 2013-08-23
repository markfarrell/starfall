//Copyright (c) 2013 Mark Farrell
#pragma once

#include "Starfall\Scene.h"
#include "Starfall\Camera.h"
#include "Starfall\Model.h"
#include "Starfall\Shader.h"


#include <glm\glm.hpp>

#include <string>
using std::string;

namespace Starfall {

	class Application; 
	
	class WorldScene : public Scene { 
		friend class LoadScene;
		private:

			sf::Clock clock;

			Camera camera;
			Shader shader;
			Model model;


			GLuint modelMatrixLocation;
			GLuint viewMatrixLocation;
			GLuint eyeLocation;


			virtual void initialize();



		public:

			WorldScene(Application* parent);
			~WorldScene();

			virtual void render();
			virtual void update();
			virtual void load();
	};

};