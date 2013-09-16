//Copyright (c) 2013 Mark Farrell
#include "Starfall/GameObject.h"

using namespace Starfall;

GameObject::GameObject(Entity::Ptr pEntity, Model::Ptr pModel) {
	this->pEntity = pEntity;
	this->pModel = pModel;
}