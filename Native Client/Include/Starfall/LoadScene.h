//Copyright (c) 2013 Mark Farrell
#pragma once

#include "Starfall\Scene.h"

#include <Poco\Thread.h>

#include <SFML\System.hpp>
#include <SFML\Graphics.hpp>

namespace Starfall {

	class Application; 
	
	class LoadScene : public Scene { 
		private:

			
			bool isLoading;
			
			Poco::Thread resourceThread;
			sf::Clock resourceClock;
			sf::Int32 resourceTime;

			sf::Font font;
			sf::Text text;

			virtual void initialize();

		public:
			virtual void render();
			virtual void update();
			virtual void load();

			LoadScene(Application* parent); //set prev and next to NULL; the application manages instances of scenes
	};

}