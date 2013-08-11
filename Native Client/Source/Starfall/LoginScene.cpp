//Copyright (c) 2013 Mark Farrell
#include "Starfall\LoginScene.h"
#include "Starfall\Application.h"
#include "Starfall\LoginUI.h"
#include "Starfall\LoginControls.h"


using namespace Starfall;

LoginScene::LoginScene(Application* parent) :
	Scene(parent),
	skybox(false)
{
	this->pLoginUI = new LoginUI();
}

void LoginScene::initialize() {
	   // Set the color and depth clear values
    glClearDepth(1.f);
    glClearColor(0.f, 0.f, 0.f, 1.f);

    // Enable Z-buffer read and write
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    // Disable lighting and texturing
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    // Configure the viewport (the same size as the window)
	glViewport(0, 0, this->parent->window.getSize().x, this->parent->window.getSize().y);

    // Setup a perspective projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	GLfloat ratio = static_cast<float>(this->parent->window.getSize().x) / this->parent->window.getSize().y;
    glFrustum(-ratio, ratio, -1.f, 1.f, 1.f, 866.f); //far is diagonal distance across skybox-cube (distance from 0,0,0 to oppose edge)

}

void LoginScene::render() {
	 // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Apply some transformations to rotate the cube
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(this->clock.getElapsedTime().asSeconds() * 1, 0.f, 0.f, 1.f);

	this->skybox.render(this->parent->window);
	this->pLoginUI->render(this->parent->window);
}

void LoginScene::load() {
	this->pLoginUI->initSurface();
	this->pLoginUI->center(this->parent->window.getSize());
	skybox.load();

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
}

LoginScene::~LoginScene() {
	delete this->pLoginUI;
}
