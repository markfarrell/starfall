//Copyright (c) 2013 Mark Farrell
#pragma once

#include "Starfall\ConfigurationFile.h"

#include <SFML/Graphics.hpp>

#include <Awesomium/WebCore.h>
#include <Awesomium/BitmapSurface.h>
#include <Awesomium/STLHelpers.h>
#include <Awesomium/DataSource.h>
#include <Awesomium/DataPak.h>
#include <Awesomium/WebViewListener.h>


namespace Starfall {
	class LoginUI  { 
		friend class LoginScene;
		friend class LoginControls;
		friend class Application;
		protected:

			ConfigurationFile config;

			Awesomium::WebCore* core; //TODO: UIs should share a core; move core outside of LoginUI
			Awesomium::WebView* view;
			Awesomium::DataSource* dataSource;

			/*****************/
			/*TODO: All UIs will have these data attributes. */
			sf::Clock uiClock;
			float maxFps;
			float uiResetTime;

			sf::Image surfaceImage;
			sf::Texture surfaceTexture;
			sf::Sprite surfaceSprite;
			/*****************/

			LoginControls* controls;

			void initSurface();
			void updateSurface();

			bool contains(sf::Vector2f& mouseVector);
			void center(sf::Vector2u windowSize);
			void render(sf::RenderWindow& window);

			LoginUI();
			~LoginUI(); //deletes controls; destroys view
	};
}