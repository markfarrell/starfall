//Copyright (c) 2013 Mark Farrell
#pragma once

#include "Starfall\Scene.h"
#include "Starfall\Skybox.h"

namespace Starfall {

	class Application; 
	class LoginUI;

	class LoginScene : public Scene { 
		protected:
			virtual void initialize();
		public:

			sf::Clock clock;
			Skybox skybox;
			LoginUI* pLoginUI;

			LoginScene(Application* parent);
			~LoginScene();

	
			virtual void render();
			virtual void update();
			virtual void load();
	};
}