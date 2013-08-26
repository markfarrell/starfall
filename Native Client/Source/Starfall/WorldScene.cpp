//Copyright (c) 2013 Mark Farrell
#include "Starfall/WorldScene.h"

#include "Starfall/Application.h"

#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <math.h>
#include <iostream>

using std::cout;
using std::endl;

using namespace Starfall;

WorldScene::WorldScene(Application* parent)
	: Scene(parent)
{
	this->model = Model::Create("Models/Humanoid.json");
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

	this->model->rotation.z = clock.getElapsedTime().asSeconds()*5.0f;
	this->model->apply();
	this->model->render(this->technique);

	glUseProgram(NULL);
}

void WorldScene::load() {

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

		this->camera.controls.update(event);
	}

	this->camera.controls.apply();

}