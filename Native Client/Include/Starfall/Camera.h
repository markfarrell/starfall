//Copyright (c) 2013 Mark Farrell
#pragma once

#include <Poco/Foundation.h>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <string>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


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

			void applyToState(string stateName, glm::vec3& deltaPosition, glm::vec3& deltaRotation);


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
		glm::vec3 direction; //the direction that the camera is pointing in

		glm::vec3 eye;
		glm::vec3 lookat;
		glm::vec3 up; 

		glm::vec3 rotation;
		glm::mat4 view;


		Camera();

		void initialize(sf::RenderWindow& window);
		void recalculate(); //updates the camera's direction once; can then be used several times without recalculation

		
		inline float theta() {
			return this->rotation.x;
		}

		inline float phi() {
			return this->rotation.y;
		}



		static void Clamp(float& angle, float delta); 

	};

}