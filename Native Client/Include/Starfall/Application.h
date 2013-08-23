//Copyright (c) 2013 Mark Farrell
#pragma once
#include "Starfall\ConfigurationFile.h"
#include "Starfall\Skybox.h"
#include "Starfall\Loadable.h"


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


#include <string>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <vector>

using std::vector;
using std::string;


namespace Starfall {

	class Scene;
	class LoadScene;
	class LoginScene;
	class WorldScene;

	class Application {
		public:

			vector<Loadable*> loadables; //Models, Shaders ... will be stored and access elsewhere; store pointers to the common base class
			//TODO: add thread pool to manage how resources in this collect are loaded concurrently

			ConfigurationFile config;

			Skybox skybox; //skybox shared by both scenes

			Scene* currentScene; 
			LoadScene* loadScene;
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
