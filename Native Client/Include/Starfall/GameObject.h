//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).
#pragma once

#include "Starfall/Entity.h"
#include "Starfall/Model.h"
#include "Starfall/TransformEntityStruct.h"


#include <SFML/System.hpp>
#include <Poco/SharedPtr.h>


namespace Starfall {

	class GameObject {

		friend class WorldScene;

		protected:

			Entity::Ptr pEntity;
			Model::Ptr pModel;

			virtual void update();

		public:
			typedef Poco::SharedPtr<GameObject> Ptr;

			GameObject(Entity::Ptr pEntity, Model::Ptr pModel);

	};


	class Target {
		public:
			TransformStruct endTransform;
			TransformStruct beginTransform;
			float rotateSpeed;
			float translateSpeed;
			bool initialized; 
			Target();
	};

	class RemoteControlledGameObject : public GameObject { 

		private:

			sf::Clock clock;
			sf::Int32 pathInterval;

			Target target;

			void next();

		public:
			typedef Poco::SharedPtr<RemoteControlledGameObject> Ptr;

			RemoteControlledGameObject(Entity::Ptr pEntity, Model::Ptr pModel);

			virtual void update();


	};

}