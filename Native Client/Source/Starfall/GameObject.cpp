//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).
#include "Starfall/GameObject.h"

#include "Starfall/ConfigurationFile.h"

using namespace Starfall;

GameObject::GameObject(Entity::Ptr pEntity, Model::Ptr pModel) {
	this->pEntity = pEntity;
	this->pModel = pModel;
}

void GameObject::update() {}

Target::Target() 
	: initialized(false) {
}

RemoteControlledGameObject::RemoteControlledGameObject(Entity::Ptr pEntity, Model::Ptr pModel) 
	: GameObject(pEntity, pModel),
	  pathInterval(ConfigurationFile::Client().getInt("path.interval")) {
	this->pEntity = pEntity;
	this->pModel = pModel;
}

void RemoteControlledGameObject::next() {

	if(this->pEntity->isPathEmpty()) { //Do not not set a new target if there are no targets available.
		return;
	}

	Target target;
	Transform entityTransform = this->pEntity->getTransform(); //thread-safe operation
	float angleBetween;

	target.beginTransform.action = this->target.endTransform.action;
	target.beginTransform.position = entityTransform.position;
	target.beginTransform.orientation = entityTransform.orientation;

	target.endTransform = this->pEntity->popPath();

	angleBetween = 2.0f * glm::acos(glm::dot(target.beginTransform.orientation,target.endTransform.orientation)); //compute angle between quaternions using vector dot product so that rotational speed can be calculated.

	target.rotateSpeed = angleBetween/float(pathInterval);

	target.initialized = true;

	this->target = target;

	this->clock.restart();
}

void RemoteControlledGameObject::update() { //move remote controlled entity
	if(this->clock.getElapsedTime().asMilliseconds() >= this->pathInterval) {
		if(this->target.initialized) {
			this->pEntity->setTransform(this->target.endTransform);
		}
		this->next();
	} else {
		if(this->target.initialized) {

			float timeFraction;
			float orientationSlerpScalar; 
			float translateLerpScalar;
			Transform currentTransform;

			timeFraction = float(this->clock.getElapsedTime().asMilliseconds())/float(this->pathInterval);
			orientationSlerpScalar = timeFraction*this->target.rotateSpeed;
			translateLerpScalar = timeFraction*this->target.translateSpeed;

			currentTransform.position = this->target.beginTransform.position + (this->target.endTransform.position - this->target.beginTransform.position)*translateLerpScalar; //move towards
			currentTransform.orientation = glm::slerp(this->target.beginTransform.orientation, this->target.endTransform.orientation, orientationSlerpScalar); //rotate towards

			this->pEntity->setTransform(currentTransform);

		} 
	}
}