//Copyright (c) 2013 Mark Farrell

#include "Starfall/Camera.h"
#include "Starfall/ConfigurationFile.h"
#include "Starfall/Assets.h"

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <math.h>

using namespace Starfall;

CameraControls::CameraControls(Camera* parent)
	: moveSpeed(2.0f),
	  rotateSpeed(90.0f) //2 * 20 = 40 degrees per second
{
	this->parent = parent;
	this->states["pitch.negative"] = 0; //pitch = x rotation
	this->states["pitch.positive"] = 0; 
	this->states["yaw.negative"] = 0; //yaw = y rotation
	this->states["yaw.positive"] = 0;
	this->states["roll.negative"] = 0; //roll = z rotation
	this->states["roll.positive"] = 0;
	this->states["move.forward"] = 0; 
}

void CameraControls::update(sf::Event& event) { 

	if(event.type == sf::Event::KeyPressed ||
		event.type == sf::Event::KeyReleased ) {
		
		bool keyPressed = event.type==sf::Event::KeyPressed; //key released if not true

		switch(event.key.code) {
			case sf::Keyboard::Q:
				this->configureState("pitch.negative", keyPressed);
			break;
			case sf::Keyboard::E:
				this->configureState("pitch.positive", keyPressed);
			break;
			case sf::Keyboard::A:
				this->configureState("yaw.negative", keyPressed);
			break;
			case sf::Keyboard::D:
				this->configureState("yaw.positive", keyPressed);
			break;
			case sf::Keyboard::Z:
				this->configureState("roll.negative", keyPressed);
			break;
			case sf::Keyboard::C:
				this->configureState("roll.positive", keyPressed);
			break;
			case sf::Keyboard::W:
				this->configureState("move.forward", keyPressed);
			break;
			case sf::Keyboard::S:
				this->configureState("move.forward", keyPressed);
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
	if(this->applyClock.getElapsedTime().asMilliseconds()) {

		glm::vec3 zero = glm::zero<glm::vec3>();

		//this->applyToState("pitch.negative", zero, sf::Vector3f(-1.0f, 0.0f, 0.0f));
		//this->applyToState("pitch.positive", zero, sf::Vector3f(1.0f, 0.0f, 0.0f));
		this->applyToState("yaw.negative", zero, glm::vec3(0.0f, -1.0f,0.0f));
		this->applyToState("yaw.positive", zero, glm::vec3(0.0f, 1.0f, 0.0f));
		this->applyToState("roll.negative", zero, glm::vec3(-1.0f, 0.0f, 0.0f));
		this->applyToState("roll.positive", zero, glm::vec3(1.0f, 0.0f, 0.0f));

		this->parent->recalculate();

		this->applyToState("move.forward", this->parent->direction, zero);
		this->applyToState("move.backward", -this->parent->direction, zero);
		
		this->applyClock.restart();
	}
}

void CameraControls::applyToState(string stateName, glm::vec3& deltaPosition, glm::vec3& deltaRotation) {

	float scalar = float(this->applyClock.getElapsedTime().asMilliseconds())/1000.0f;
	float rotateAmount = this->rotateSpeed*scalar;
	switch(this->states[stateName]) {
			case 1:
				this->parent->eye += deltaPosition*this->moveSpeed*scalar;
				this->parent->rotation += deltaRotation*rotateAmount;
				Camera::Clamp(this->parent->rotation.x,rotateAmount);
				Camera::Clamp(this->parent->rotation.y, rotateAmount);
				Camera::Clamp(this->parent->rotation.z, rotateAmount);
			break;
			case 2:
				this->states[stateName] = 0;
			break;
	}
}

void Camera::Clamp(float& angle, float delta) {
	if(angle < 0.0f) { 
		angle = 360.0f-abs(delta);
	} else if(angle > 360.0f) {
		angle = 0.0f+abs(delta);
	}
}

Camera::Camera()
	: distance(2.0f),
	up(0.0f,1.0f,0.0f),
	controls(this) 
{

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

	float xfov = float(ConfigurationFile::Client().getInt("view.xfov"));
	float yfov = glm::degrees(2.0f * glm::atan(glm::tan(glm::radians(xfov * 0.5f)) / ratio));
	float size = float(ConfigurationFile::Client().getInt("view.size"));
	float nearClip = 1.0f;
	float farClip = glm::distance(glm::zero<glm::vec3>(), glm::vec3(size,size,size)); //866.f
	//glFrustum(-ratio, ratio, -1.f, 1.f, nearClip, farClip); //far is diagonal distance across skybox-cube (distance from 0,0,0 to oppose edge)
	gluPerspective(yfov, ratio, nearClip, farClip); //866.f
}

void Camera::recalculate() {

		double conversion = 3.14159265 / 180.0; //degrees to radians

		this->direction = glm::vec3(
			float(cos(this->theta()*conversion)*cos(this->phi()*conversion)),
			float(sin(this->theta()*conversion)),
			float(cos(this->theta()*conversion)*sin(this->phi()*conversion))
		); 
		this->direction *= 1.0f / sqrt(this->direction.x*this->direction.x + this->direction.y*this->direction.y + this->direction.z*this->direction.z);
		

		this->lookat = this->eye+this->direction;
		this->up = glm::vec3(glm::rotate(glm::mat4(1.0), this->rotation.z, glm::vec3(0.f,0.f,1.f)) * glm::vec4(0.0f, (cos(this->theta()*conversion) > 0.0f) ? 1.0f : -1.0f, 0.0f, 1.0f)); 


		//Note: Be careful of choosing up direction: an exception (divide by zero) will occur if up and center-eye are in the same direction. The cross product of these vectors can't be taken if they have the same value.

		this->view = glm::mat4(1.0f);
		this->view = glm::lookAt(	
			glm::vec3(this->eye.x, this->eye.y, this->eye.z), 
			glm::vec3(lookat.x, this->lookat.y, this->lookat.z),
			glm::vec3(this->up.x, this->up.y, this->up.z)
		);

}