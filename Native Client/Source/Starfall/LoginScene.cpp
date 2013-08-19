//Copyright (c) 2013 Mark Farrell
#include "Starfall\LoginScene.h"

#include "Starfall\Client.h"
#include "Starfall\Application.h"
#include "Starfall\LoginUI.h"
#include "Starfall\LoginControls.h"

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

using namespace Starfall;



LoginScene::LoginScene(Application* parent)
	: Scene(parent)
{
	this->pLoginUI = new LoginUI();
}

void LoginScene::initialize() {
	this->camera.initialize(this->parent->window);
}

void LoginScene::render() {
	 // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Apply some transformations to rotate the cube
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(this->clock.getElapsedTime().asSeconds() * 1, 0.f, 0.f, 1.f);

	this->parent->skybox.render(this->parent->window);
	this->pLoginUI->render(this->parent->window);
}

void LoginScene::load() {
	this->pLoginUI->initSurface();
	this->pLoginUI->center(this->parent->window.getSize());
}

void LoginScene::update() {
	// Process events
	sf::Event event;
	while (this->parent->window.pollEvent(event))
	{
		// Close window: exit
		if (event.type == sf::Event::Closed) {
			this->parent->window.close();
		}

		if(event.type == sf::Event::MouseMoved) {
			this->pLoginUI->controls->mouseMove(event);
		}

		if(event.type == sf::Event::MouseButtonPressed) {
			this->pLoginUI->controls->mouseButtonPressed(event);
		}

		if(event.type == sf::Event::MouseButtonReleased) {
			this->pLoginUI->controls->mouseButtonReleased(event);
		}

		// Escape key: exit
		if ((event.type == sf::Event::KeyPressed))
		{
			if(event.key.code == sf::Keyboard::Escape) {
				this->parent->window.close();
			} 
			this->pLoginUI->controls->keyEvent(Awesomium::WebKeyboardEvent::kTypeKeyDown, event);
		}

		if ((event.type == sf::Event::KeyReleased))
		{
			this->pLoginUI->controls->keyEvent(Awesomium::WebKeyboardEvent::kTypeKeyUp, event);
		}

		if(event.type == sf::Event::TextEntered) {
			this->pLoginUI->controls->textEvent(event);
		}

		// Resize event: adjust the viewport
		if (event.type == sf::Event::Resized) {
			glViewport(0, 0, event.size.width, event.size.height);
			this->parent->window.setView(sf::View(sf::Vector2f(float(event.size.width/2), float(event.size.height/2)), sf::Vector2f(float(event.size.width), float(event.size.height))));
			this->pLoginUI->center(this->parent->window.getSize());
		}

	}

	this->pLoginUI->updateSurface();

	if(Client::Get()->isLoggedIn()) {
		if(this->pLoginUI->transitioned()) { //returns true when done transitioning
			this->next();
		}
	}

}

LoginScene::~LoginScene() {
	delete this->pLoginUI;
}
