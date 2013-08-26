//Copyright (c) 2013 Mark Farrell

#include "Starfall/Camera.h"
#include "Starfall/ConfigurationFile.h"
#include "Starfall/Assets.h"

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

#include <math.h>

using namespace Starfall;

CameraControls::CameraControls(Camera* parent)
	: moveSpeed(2.0f),
	  rotateSpeed(90.0f), //2 * 20 = 40 degrees per second
	  applyTime(20)
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
				this->configureState("yaw.positive", keyPressed);
			break;
			case sf::Keyboard::D:
				this->configureState("yaw.negative", keyPressed);
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

	if(event.type == sf::Event::MouseWheelMoved) {
		this->parent->distance = glm::clamp(
			this->parent->distance + float(-event.mouseWheel.delta)*0.1f,
			this->parent->minimumDistance(), 
			this->parent->maximumDistance()
		);
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

		glm::vec3 zero = glm::zero<glm::vec3>();

		this->applyToState("pitch.negative", zero, glm::vec3(-1.0f, 0.0f, 0.0f));
		this->applyToState("pitch.positive", zero, glm::vec3(1.0f, 0.0f, 0.0f));
		this->applyToState("yaw.negative", zero, glm::vec3(0.0f, -1.0f,0.0f));
		this->applyToState("yaw.positive", zero, glm::vec3(0.0f, 1.0f, 0.0f));
		this->applyToState("roll.negative", zero, glm::vec3(0.0f, 0.0f, -1.0f));
		this->applyToState("roll.positive", zero, glm::vec3(0.0f, 0.0f, 1.0f));


		this->applyToState("move.forward", this->parent->direction, zero);
		this->applyToState("move.backward", -this->parent->direction, zero);

		this->parent->recalculate();
		
		this->applyClock.restart();
	}
}

void CameraControls::applyToState(string stateName, glm::vec3& deltaPosition, glm::vec3& deltaRotation) {

	float scalar = float(this->applyClock.getElapsedTime().asMilliseconds())/1000.0f;
	deltaRotation *= this->rotateSpeed*scalar;
	deltaPosition *= this->moveSpeed*scalar;

	switch(this->states[stateName]) {
			case 1:
				this->parent->position += deltaPosition;
				this->parent->orientation = glm::rotate(this->parent->orientation, deltaRotation.y, glm::vec3(0.0f,1.0f,0.0f)); //yaw 
				this->parent->orientation = glm::rotate(this->parent->orientation, deltaRotation.x, glm::vec3(1.0f,0.0f,0.0f)); //pitch 
				this->parent->orientation = glm::rotate(this->parent->orientation, deltaRotation.z, glm::vec3(0.0f,0.0f,1.0f)); //roll
			break;
			case 2:
				this->states[stateName] = 0;
			break;
	}
}



Camera::Camera()
	: distance(2.0f),
	controls(this) 
{
	float size = float(ConfigurationFile::Client().getInt("view.size")); //skybox size
	this->nearClip = 1.0f;
	this->farClip = glm::distance(glm::zero<glm::vec3>(), glm::vec3(size,size,size)); //866.f; diagonal distance across skybox
	this->projection = glm::mat4(1.0f); //calculated in Camera::initialize()
}

void Camera::initialize(sf::RenderWindow& window) { 
	GLfloat ratio = static_cast<float>(window.getSize().x) / static_cast<float>(window.getSize().y);
	this->projection = glm::frustum(-ratio, ratio, -1.f, 1.f, this->nearClip, this->farClip);
	
    glClearDepth(1.f);
    glClearColor(0.f, 0.f, 0.f, 1.f);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

	glViewport(0, 0, window.getSize().x, window.getSize().y);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

	glLoadMatrixf(glm::value_ptr(this->projection));
}

void Camera::recalculate() {

		this->direction = glm::vec3(this->orientation * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));

		if(glm::length2(this->direction) > 0.0f) {
			this->direction = glm::normalize(this->direction);
		}

		glm::vec3 lookat(this->position+this->direction);

		glm::vec3 up = glm::vec3(this->orientation * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));

		if(glm::length2(up) > 0.0f) {
			up = glm::normalize(up);
		}

		glm::vec3 eye = this->position-this->direction*(glm::max(this->distance-1.0f, 0.0f));

		this->view = glm::lookAt(eye, lookat, up);
}

float Camera::yfov(float xfov, float ratio) {
	return glm::degrees(2.0f * glm::atan(glm::tan(glm::radians(xfov * 0.5f)) / ratio));
}