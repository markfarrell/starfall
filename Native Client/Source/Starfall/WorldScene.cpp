//Copyright (c) 2013 Mark Farrell
#include "Starfall/WorldScene.h"

#include "Starfall/Application.h"

#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <math.h>
#include <fstream>
#include <iostream>

using std::cout;
using std::endl;

using namespace Starfall;

WorldScene::WorldScene(Application* parent)
	: Scene(parent),
	  model("Models/Humanoid.json"),
	  vertexShaderID(NULL),
	  fragmentShaderID(NULL),
	  programID(NULL)
{
}

WorldScene::~WorldScene() {
	glDetachShader(this->programID, this->vertexShaderID);
	glDetachShader(this->programID, this->fragmentShaderID);
	glDeleteShader(this->vertexShaderID);
	glDeleteShader(this->fragmentShaderID);
	glDeleteProgram(this->programID);
}

void WorldScene::initialize() { 
	this->camera.initialize(this->parent->window);
	this->camera.eye = glm::vec3(0.0f, 0.0f, 0.0f);

}

void WorldScene::render() {
	

	glUseProgram(NULL);

	// Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // Apply some transformations to rotate the cube
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glLoadMatrixf(glm::value_ptr(this->camera.view));

	this->parent->skybox.position = this->camera.eye;
	this->parent->skybox.render(this->parent->window);

	//Use a map to store uniform values; add shader to model class
	glUseProgram(this->programID);
	glUniformMatrix4fv(this->modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(this->model.matrix));
	glUniformMatrix4fv(this->viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(this->camera.view));
	glUniform3f(this->eyeLocation, 	this->camera.eye.x, this->camera.eye.y, this->camera.eye.z);

	this->model.rotation.z = clock.getElapsedTime().asSeconds()*5.0f;
	this->model.apply();
	this->model.render();

	glUseProgram(NULL);
	
}

GLuint WorldScene::createShader(GLuint type, string path) { 
	GLuint shaderID = glCreateShader(type);
	std::ifstream shaderStream(Assets::Path(path));
	if(shaderStream.is_open()) {
		string shaderSource;

		const char** shaderSourceArray = new const char*[1];
		int* shaderLengthArray = new int[1];


		shaderStream.seekg(0, std::ios::end);
		shaderSource.resize((unsigned int)(shaderStream.tellg()));
		shaderStream.seekg(0, std::ios::beg);
		shaderStream.read(&shaderSource[0], shaderSource.size());

	
		shaderSourceArray[0] = shaderSource.c_str();
		shaderLengthArray[0] = shaderSource.size();

		glShaderSource(shaderID, 1, shaderSourceArray, shaderLengthArray);
		glCompileShader(shaderID);

		int result = 0;
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
		if(result != GL_TRUE) {
				string infoLog;
				int infoLogLength = 0;
				int charsWritten = 0;
				glGetShaderiv(this->vertexShaderID, GL_INFO_LOG_LENGTH,&infoLogLength);
				if (infoLogLength > 0)
				{
					infoLog.resize(infoLogLength);
					glGetShaderInfoLog(shaderID, infoLogLength, &charsWritten, &infoLog[0]);
					cout << "[World Scene::createShader] " << infoLog << endl;
				}
		}


		shaderStream.close();

		delete shaderSourceArray;
		delete shaderLengthArray;
	} else {
		//Could not open file
	}
	return shaderID;
}

GLuint WorldScene::createProgram() { 
	GLuint id = glCreateProgram();
	glAttachShader(id, this->vertexShaderID);
	glAttachShader(id, this->fragmentShaderID);
	glLinkProgram(id);
	return id;
}

void WorldScene::load() {
	this->vertexShaderID = this->createShader(GL_VERTEX_SHADER, "Shaders/Shader.vert");
	this->fragmentShaderID = this->createShader(GL_FRAGMENT_SHADER, "Shaders/Shader.frag");
	this->programID = this->createProgram();
	this->modelMatrixLocation = glGetUniformLocation(this->programID, "modelMatrix");
	this->viewMatrixLocation = glGetUniformLocation(this->programID, "viewMatrix");
	this->eyeLocation = glGetUniformLocation(this->programID, "eye");
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
			this->camera.initialize(this->parent->window);
			this->parent->window.setView(sf::View(sf::Vector2f(float(event.size.width/2), float(event.size.height/2)), sf::Vector2f(float(event.size.width), float(event.size.height))));
		}

		this->camera.controls.update(event);
	}

	this->camera.controls.apply();

}