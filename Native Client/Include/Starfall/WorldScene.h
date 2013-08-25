//Copyright (c) 2013 Mark Farrell
#pragma once

#include "Starfall\Scene.h"
#include "Starfall\Camera.h"
#include "Starfall\Model.h"
#include "Starfall\ToonTechnique.h"

#include <glm\glm.hpp>

#include <string>
#include <map>

using std::string;
using std::map;

namespace Starfall {

	class Application; 
	
	class WorldScene : public Scene { 
		friend class LoadScene;
		private:

			sf::Clock clock;

			Camera camera;
			Technique::Ptr technique;
			Model::Ptr model;


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