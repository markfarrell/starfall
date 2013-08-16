//Copyright (c) 2013 Mark Farrell
#pragma once


#include <string>
#include <algorithm>
#include <cmath>
#include <fstream>

#include <Windows.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include <Awesomium/WebCore.h>
#include <Awesomium/BitmapSurface.h>
#include <Awesomium/STLHelpers.h>
#include <Awesomium/DataSource.h>
#include <Awesomium/DataPak.h>
#include <Awesomium/WebViewListener.h>



#include "Starfall\ConfigurationFile.h"
#include "Starfall\Skybox.h"


using std::string;


namespace Starfall {

	class Scene;
	class LoginScene;
	class WorldScene;

	class Application {
		public:
			ConfigurationFile config;

			Scene* currentScene; 
			LoginScene* loginScene;
			WorldScene* worldScene;
			sf::RenderWindow window;

			void changeScene(Scene* scene);
			void logout();
			void update();
			void render();
			Application();
			~Application(); //shutsdown web core
			void run();

			bool checkRequirements(); //Check to make sure the the required OpenGL version is available, and that GLEW has initialized. Return false if these conditions are not met.
	};
}
