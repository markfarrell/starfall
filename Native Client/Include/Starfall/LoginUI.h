//Copyright (c) 2013 Mark Farrell
#pragma once

#include "Starfall\ConfigurationFile.h"
#include "Starfall\Assets.h"

#include <SFML/Graphics.hpp>

#include <Awesomium/WebCore.h>
#include <Awesomium/BitmapSurface.h>
#include <Awesomium/STLHelpers.h>
#include <Awesomium/DataSource.h>
#include <Awesomium/DataPak.h>
#include <Awesomium/WebViewListener.h>

#include <Poco\Mutex.h>

#include <stack>
#include <string>

using std::string;


namespace Starfall {

	class Transition {
		private:
		
			bool active;

			sf::Clock resetClock;
			sf::Int32 resetTime;

			sf::Clock motionClock;
			sf::Int32 motionTime;
			
		public:
			Transition();
			void activate();
			bool isActive();
			bool isDone();
			bool move(); //returns whether should an object be animated one step based on the time of motion clock
			float percentageSpeed(); //speed at which an object should; amount to move each step to synchronize with the transition's reset clock
	};

	class LoginUI  { 
		friend class LoginScene;
		friend class LoginControls;
		friend class Application;
		protected:

			ConfigurationFile config;

			Awesomium::WebCore* core; //TODO: UIs should share a core; move core outside of LoginUI
			Awesomium::WebView* view;
			Awesomium::DataSource* dataSource;

			Transition transition;

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

			Poco::Mutex statusMutex;
			std::stack<string> statusStack; //status messages stack from the connection thread; read the most recent one and clear

			void initSurface();
			void updateSurface();

			bool contains(sf::Vector2f& mouseVector);
			void center(sf::Vector2u windowSize);
			void render(sf::RenderWindow& window);

			bool transitioned();

			void updateStatus(); //executes JavaScript in the web control; accesses the top of the status stack; should be called in the same thread as view.
			void setStatus(string status); //sets the login status message displayed on the interface

			LoginUI();
			~LoginUI(); //deletes controls; destroys view
	};
}