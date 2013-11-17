//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).
#include "Starfall/WorldScene.h"

#include "Starfall/Application.h"
#include "Starfall/Client.h"
#include "Starfall/ConfigurationFile.h"

#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/norm.hpp>

#include <math.h>
#include <iostream>

using std::cout;
using std::endl;

using namespace Starfall;

WorldScene::WorldScene(Application* parent)
	: Scene(parent),
	pathInterval(ConfigurationFile::Client().getInt("path.interval")),
	updateInterval(ConfigurationFile::Client().getInt("world.updateInterval")),
	humanoidModelPath(ConfigurationFile::Client().getString("world.humanoidModelPath"))
{
	this->humanoidModel = Model::Create(this->humanoidModelPath);
	this->technique = ToonTechnique::Create();
}

WorldScene::~WorldScene() {

}

void WorldScene::initialize() { 
	this->camera.initialize(this->parent->window);
}

void WorldScene::render() {
	glUseProgram(NULL);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glLoadMatrixf(glm::value_ptr(this->camera.view));

	this->parent->skybox.position = this->camera.position;
	this->parent->skybox.render(this->parent->window);

	this->humanoidModel->render(this->technique);

	for(Poco::HashMap<Poco::UInt32, GameObject::Ptr>::Iterator it = this->objectsMap.begin(); it != this->objectsMap.end(); it++) {
		GameObject::Ptr& gameObject = (*it).second;
		gameObject->pModel->render(this->technique);
	}

	glUseProgram(NULL);
}

void WorldScene::load() {

}

bool WorldScene::checkPlayer() {
	bool ret = false;
	if(Client::Get()->isLoggedIn()) {
		Poco::ScopedLock<Poco::Mutex> lock(Client::Get()->pPlayer->mutex());
		if(this->objectsMap.find(Client::Get()->pPlayer->pEntity->sessionid) != this->objectsMap.end()) {
			ret = true;
		}
	}
	return ret;
}

void WorldScene::update() {
	sf::Event event;
	while (this->parent->window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed) {
			this->parent->window.close();
		}

		if (event.type == sf::Event::Resized) {
			this->camera.initialize(this->parent->window);
			this->parent->window.setView(sf::View(sf::Vector2f(float(event.size.width/2), float(event.size.height/2)), sf::Vector2f(float(event.size.width), float(event.size.height))));
		}

		//TODO: Move to model.controls
		if(event.type == sf::Event::KeyPressed) {
			if(event.key.code == sf::Keyboard::F1) {
				this->humanoidModel->states["debug.boundingboxes"] = !this->humanoidModel->states["debug.boundingboxes"];
			}
		}

		this->camera.controls.update(event);
	}

	//TODO: Move to model.controls
	if(this->camera.controls.apply()) {
		this->humanoidModel->apply(this->camera);
	}

	if(this->clock.getElapsedTime().asMilliseconds() >= this->updateInterval) {

		Player::Ptr pPlayer = Client::Get()->pPlayer;

		vector<CreateEntityStruct> createEntities;
		vector<TransformEntityStruct> transformEntities;
		vector<DestroyEntityStruct> destroyEntities;

		{ //Lock, copy, then unlock to free up access to the player in networking more quickly
			Poco::ScopedLock<Poco::Mutex> lock(pPlayer->mutex());
			
			createEntities.insert(createEntities.begin(), pPlayer->createEntityQueue.begin(), pPlayer->createEntityQueue.end());
			transformEntities.insert(transformEntities.begin(), pPlayer->transformEntityQueue.begin(), pPlayer->transformEntityQueue.end());
			destroyEntities.insert(destroyEntities.begin(), pPlayer->destroyEntityQueue.begin(), pPlayer->destroyEntityQueue.end());
		}

		for(vector<CreateEntityStruct>::iterator it = createEntities.begin(); it != createEntities.end(); it++) {
			if(this->objectsMap.find((*it).sessionid) != this->objectsMap.end()) {
				cout << "[WorldScene::update] Warning: Entity " << (*it).sessionid << " has already been created." << endl;
			} else {

				Entity::Ptr pEntity;
				
				{
					Poco::ScopedLock<Poco::Mutex> lock(Client::Get()->pPlayer->mutex());
					if(Client::Get()->isLoggedIn() &&
						(*it).sessionid == Client::Get()->pPlayer->pEntity->sessionid) {
						pEntity = Client::Get()->pPlayer->pEntity;
					} else {
						pEntity = Entity::Create((*it).sessionid);
					}
				}


				this->objectsMap[(*it).sessionid] = GameObject::Ptr(new GameObject(pEntity, Model::Ptr(new Model(this->humanoidModel)))); //TODO: RemoteControlledGameObject for non-player entities
			}
		}

		for(vector<TransformEntityStruct>::iterator it = transformEntities.begin(); it != transformEntities.end(); it++) {
			if(this->objectsMap.find((*it).sessionid) != this->objectsMap.end()) {
				for(vector<TransformStruct>::iterator pathIterator = (*it).path.begin(); pathIterator != (*it).path.end(); pathIterator++) {
					//this->objectsMap[(*it).sessionid]->pEntity->addToPath((*pathIterator));
				}
			}
		}

		for(vector<DestroyEntityStruct>::iterator it = destroyEntities.begin(); it != destroyEntities.end(); it++) {
			if(this->objectsMap.find((*it).sessionid) != this->objectsMap.end()) {
				this->objectsMap.erase((*it).sessionid);
			}
		}

		for(Poco::HashMap<Poco::UInt32, GameObject::Ptr>::Iterator it = this->objectsMap.begin(); it != this->objectsMap.end(); it++) {
			GameObject::Ptr& gameObject = (*it).second;
			gameObject->update();
		}
	
		this->clock.restart();
	}


}
