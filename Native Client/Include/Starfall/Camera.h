//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).
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

			sf::Vector2f previousMouseLocation; //To rotate the camera with the mouse, the direction that the user is moving the camera must be tracked.

			void applyToState(string stateName, glm::vec3& deltaPosition, glm::vec3& deltaRotation);

		public: 
			CameraControls(Camera* parent);
			void update(sf::Event& event);
			void configureState(string stateName, bool& keyPressed); //0->1 : action activated on keypress, 1->2: action signalled to stop on keyrelease; 2->0:  when action is finished
			bool apply(); //returns true if recalculations were made
	};

	class Camera {

		friend class WorldScene;
		friend class Model;
		friend class CameraControls;

		private:

			/** Recalculated members **/
			glm::mat4 projection;
			glm::mat4 defaultView;
			glm::mat4 view;
			glm::vec3 direction; //direction the forward vector is pointing in world space
			glm::vec3 up;

			glm::quat offsets; //euler angle offsets for rotation after view as been calculated

			void recalculate(); //updates the camera's view and direction; they can then be used several times without recalculation

			sf::Vector2u size;

		public:

			CameraControls controls;

			float nearClip;
			float farClip;

			float distance; //distance to place camera behind its lookat position

			glm::quat orientation;
			glm::vec3 position;

			Camera();

			void initialize(sf::RenderWindow& window);

			inline float minimumDistance() { return 4.0f; } 
			inline float maximumDistance() { return 10.0f; }

			static float yfov(float xfov, float ratio); //utility function to calculate yfov from xfov

	};

}