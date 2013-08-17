//Copyright (c) 2013 Mark Farrell

#include "Starfall/Camera.h"

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

using namespace Starfall;

CameraControls::CameraControls(Camera* parent)
	: applyTime(50),
	  rotateSpeed(2.0f) //2 * 20 = 40 degrees per second
{
	this->parent = parent;
	this->states["pitch.negative"] = 0; //pitch = x rotation
	this->states["pitch.positive"] = 0; 
	this->states["yaw.negative"] = 0; //yaw = y rotation
	this->states["yaw.positive"] = 0;
	this->states["roll.negative"] = 0; //roll = z rotation
	this->states["roll.positive"] = 0;


}

void CameraControls::update(sf::Event& event) { 

	if(event.type == sf::Event::KeyPressed ||
		event.type == sf::Event::KeyReleased ) {
		
		bool keyPressed = event.type==sf::Event::KeyPressed; //key released if not true

		switch(event.key.code) {
			case sf::Keyboard::W:
				this->configureState("pitch.negative", keyPressed);
			break;
			case sf::Keyboard::S:
				this->configureState("pitch.positive", keyPressed);
			break;

		}

	}
}

void CameraControls::configureState(string stateName, bool& keyPressed) {
	if(keyPressed) {
		if(this->states[stateName] == 0) {
			this->states[stateName] = 1;
		}
	} else {
		if(this->states[stateName] == 1) {
			this->states[stateName] = 2;
		}
	}
}

void CameraControls::apply() {
	if(this->applyClock.getElapsedTime().asMilliseconds() >= this->applyTime) {
		this->applyClock.restart();

		//TODO: Replace with function that takes delta parameters (posX,posY,posZ,rotX,rotY,rotZ)
		switch(this->states["pitch.negative"]) {
			case 1:
				this->parent->rotation.x -= this->rotateSpeed;
			break;
			case 2:
				this->states["pitch.negative"] = 0;
			break;
		}
		//''
		switch(this->states["pitch.positive"]) {
			case 1:
				this->parent->rotation.x += this->rotateSpeed;
			break;
			case 2:
				this->states["pitch.positive"] = 0;
			break;
		}

	}
}

Camera::Camera() : controls(this) {

}

void Camera::initialize(sf::RenderWindow& window) { 
		// Set the color and depth clear values
    glClearDepth(1.f);
    glClearColor(0.f, 0.f, 0.f, 1.f);

    // Enable Z-buffer read and write
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);

    // Disable lighting and texturing
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    // Configure the viewport (the same size as the window)
	glViewport(0, 0, window.getSize().x, window.getSize().y);

    // Setup a perspective projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	GLfloat ratio = static_cast<float>(window.getSize().x) / static_cast<float>(window.getSize().y);
    glFrustum(-ratio, ratio, -1.f, 1.f, 1.f, 866.f); //far is diagonal distance across skybox-cube (distance from 0,0,0 to oppose edge)
	//gluPerspective(60.0f, ratio, 1.f, 866.f);
}