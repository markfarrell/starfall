//Copyright (c) 2013 Mark Farrell
#pragma once

#include "Starfall/Entity.h"
#include "Starfall/Model.h"

#include <Poco/SharedPtr.h>

namespace Starfall {

	class GameObject {
		friend class WorldScene;

		private:
			Entity::Ptr pEntity;
			Model::Ptr pModel;

		public:
			typedef Poco::SharedPtr<GameObject> Ptr;

			GameObject(Entity::Ptr pEntity, Model::Ptr pModel);
	};

}