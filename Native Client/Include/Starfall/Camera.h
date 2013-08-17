//Copyright (c) 2013 Mark Farrell
#pragma once

#include <Poco/Foundation.h>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <string>
#include <map>

using std::string;
using std::map;

namespace Starfall {

	class CameraControls {
		friend class Camera;
		private:
			map<string, Poco::UInt32> states;
			Camera* parent;

			float rotateSpeed;

			sf::Clock applyClock;
			sf::Int32 applyTime;
		public: 
			CameraControls(Camera* parent);
			void update(sf::Event& event);
			void configureState(string stateName, bool& keyPressed); //0->1 : action activated on keypress, 1->2: action signalled to stop on keyrelease; 2->0:  when action is finished
			void apply();
	};

	class Camera {
		public:

		CameraControls controls;

		sf::Vector3f position;
		sf::Vector3f rotation;

		Camera();

		void initialize(sf::RenderWindow& window);

	};

}