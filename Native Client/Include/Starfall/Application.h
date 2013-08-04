//Copyright (c) 2013 Mark Farrell
#pragma once

#include <algorithm>
#include <cmath>
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
#include <algorithm>
#include <fstream>

#include "Starfall\Skybox.h"


namespace Starfall {

	class Application {
		public:
			Awesomium::WebCore* core;
			Awesomium::WebView* view;
			Awesomium::DataSource* dataSource;
			Application();
			~Application();
			void run();
	};
}
