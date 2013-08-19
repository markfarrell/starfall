//Copyright (c) 2013 Mark Farrell
#include "Starfall/WorldScene.h"

#include "Starfall/Application.h"

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include <math.h>

using namespace Starfall;

WorldScene::WorldScene(Application* parent)
	: Scene(parent),
	  model("Models/Humanoid.json")
{
	this->isLoading = true;
	this->resourceTime = 500; //time to wait before checking the loading state of the model again
	this->resourceThread.start(this->model);
}

WorldScene::~WorldScene() {

}

void WorldScene::initialize() { 
	this->camera.initialize(this->parent->window);
	this->camera.eye = sf::Vector3f(0.0f, 0.0f, 0.0f);
}

void WorldScene::render() {

	// Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Apply some transformations to rotate the cube
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	


	gluLookAt(
		this->camera.eye.x, this->camera.eye.y, this->camera.eye.z, 
		this->camera.lookat.x, this->camera.lookat.y, this->camera.lookat.z, 
		this->camera.up.x, this->camera.up.y, this->camera.up.z
	); 

	this->parent->skybox.position = this->camera.eye;
	this->parent->skybox.render(this->parent->window);

	if(this->isLoading) { 

		//Draw loading screen
	} else {

		this->model.rotation.z = clock.getElapsedTime().asSeconds()*5.0f;
		this->model.render();
	}

	
}

void WorldScene::load() {
}

void WorldScene::update() {
	sf::Event event;
	while (this->parent->window.pollEvent(event))
	{
		// Close window: exit
		if (event.type == sf::Event::Closed) {
			this->parent->window.close();
		}

		// Resize event: adjust the viewport
		if (event.type == sf::Event::Resized) {
			glViewport(0, 0, event.size.width, event.size.height);
			this->parent->window.setView(sf::View(sf::Vector2f(float(event.size.width/2), float(event.size.height/2)), sf::Vector2f(float(event.size.width), float(event.size.height))));
		}

		this->camera.controls.update(event);
	}

	if(this->isLoading) {
		if(this->resourceClock.getElapsedTime().asMilliseconds() >= this->resourceTime) { //only check every half of a second to avoid too many changes in ownership over the shared resources of the model
			if(this->model.isLoaded()) {
				this->isLoading = false;
			}
			this->resourceClock.restart();
		}
	}

	this->camera.controls.apply();

}