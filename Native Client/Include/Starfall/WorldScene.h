//Copyright (c) 2013 Mark Farrell
#pragma once

#include "Starfall/Scene.h"
#include "Starfall/Camera.h"
#include "Starfall/GameObject.h"
#include "Starfall/ToonTechnique.h"

#include <Poco/Foundation.h>
#include <Poco/HashMap.h>

#include <glm/glm.hpp>

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
			Model::Ptr humanoidModel;

			Poco::HashMap<Poco::UInt32, GameObject::Ptr> objectsMap; //Maps sessionid -> gameObject for quick insertions and searches 


			GLuint modelMatrixLocation;
			GLuint viewMatrixLocation;
			GLuint eyeLocation;

			Poco::UInt32 updateInterval;
			string humanoidModelPath;


			virtual void initialize();

			bool checkPlayer(); //check to see if there exists a player object that can be controlled; a CreateEntityStruct must first be sent from the server and the player must be logged in.

		public:

			WorldScene(Application* parent);
			~WorldScene();

			virtual void render();
			virtual void update();
			virtual void load();
	};

};
