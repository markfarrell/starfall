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

			float moveSpeed;
			float rotateSpeed;

			sf::Clock applyClock;
			sf::Int32 applyTime;

			void applyToState(string stateName, sf::Vector3f& deltaPosition, float deltaTheta, float deltaPhi);



		public: 
			CameraControls(Camera* parent);
			void update(sf::Event& event);
			void configureState(string stateName, bool& keyPressed); //0->1 : action activated on keypress, 1->2: action signalled to stop on keyrelease; 2->0:  when action is finished
			void apply();
	};

	class Camera {

		public:

		CameraControls controls;


		float distance; //distance to place camera behind its lookat position
		sf::Vector3f direction; //the direction that the camera is pointing in

		float theta; //Angle between Y and Z planes
		float phi; //Angle between X and Z planes

		sf::Vector3f eye;
		sf::Vector3f lookat;
		sf::Vector3f up; 

		Camera();

		void initialize(sf::RenderWindow& window);
		void recalculate(); //updates the camera's direction once; can then be used several times without recalculation


		static void Clamp(float& angle, float delta); 

	};

}