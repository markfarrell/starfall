//Copyright (c) 2013 Mark Farrell
#pragma once

#include "Starfall\Scene.h"
#include "Starfall\Camera.h"
#include "Starfall\Model.h"

#include <Poco/Thread.h>

#include <SFML/System.hpp>


namespace Starfall {

	class Application; 
	
	class WorldScene : public Scene { 
		private:


			bool isLoading;
			
			Poco::Thread resourceThread;
			sf::Clock resourceClock;
			sf::Int32 resourceTime;

			sf::Clock clock;

			Camera camera;
			Model model;

			virtual void initialize();

		public:

			WorldScene(Application* parent);
			~WorldScene();

			virtual void render();
			virtual void update();
			virtual void load();
	};

};