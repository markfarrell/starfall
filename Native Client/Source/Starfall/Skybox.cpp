//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).
#include "Starfall/ConfigurationFile.h"
#include "Starfall/Assets.h"

#include "Starfall/Platform.h"
#include "Starfall/Skybox.h"

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

using std::cout;
using std::endl;

using namespace Starfall;

void Cube::load(glm::vec3 scale, glm::vec4 color) {
		
	                 //LEFT
		this->push(-scale.x, -scale.y, -scale.z,  -1.0f, -1.0f, -1.0f, color);
		this->push(-scale.x,  scale.y, -scale.z,  -1.0f, 1.0f, -1.0f, color);
		this->push(-scale.x, -scale.y,  scale.z,  -1.0f, -1.0f, 1.0f, color);
		this->push(-scale.x, -scale.y,  scale.z,  -1.0f, -1.0f, 1.0f, color);
		this->push(-scale.x,  scale.y, -scale.z,  -1.0f, 1.0f, -1.0f, color);
        this->push(-scale.x,  scale.y,  scale.z,  -1.0f, 1.0f, 1.0f, color);
						//RIGHT
        this->push(scale.x, -scale.y, -scale.z,  1.0, -1.0, -1.0, color);
        this->push(scale.x,  scale.y, -scale.z,  1.0, 1.0, -1.0, color);
        this->push(scale.x, -scale.y,  scale.z,  1.0, -1.0, 1.0, color);
        this->push(scale.x, -scale.y,  scale.z,  1.0, -1.0, 1.0, color);
        this->push(scale.x,  scale.y, -scale.z,  1.0, 1.0, -1.0, color);
        this->push(scale.x,  scale.y,  scale.z,  1.0, 1.0, 1.0, color);
						//DOWN
        this->push(-scale.x, -scale.y, -scale.z,  -1.0, -1.0, -1.0, color);
        this->push(scale.x, -scale.y, -scale.z,  1.0, -1.0, -1.0, color);
        this->push(-scale.x, -scale.y,  scale.z,  -1.0, -1.0, 1.0, color);
        this->push(-scale.x, -scale.y,  scale.z,  -1.0, -1.0, 1.0, color);
        this->push(scale.x, -scale.y, -scale.z,  1.0, -1.0, -1.0, color);
        this->push(scale.x, -scale.y,  scale.z,  1.0, -1.0, 1.0, color);

						//UP
        this->push(-scale.x,  scale.y, -scale.z,  -1.0, 1.0, -1.0, color);
        this->push(scale.x,  scale.y, -scale.z,  1.0, 1.0, -1.0, color);
        this->push(-scale.x,  scale.y,  scale.z,  -1.0, 1.0, 1.0, color);
        this->push(-scale.x,  scale.y,  scale.z,  -1.0, 1.0, 1.0, color);
        this->push(scale.x,  scale.y, -scale.z,  1.0, 1.0, -1.0, color);
        this->push(scale.x,  scale.y,  scale.z,  1.0, 1.0, 1.0, color);

						//BACK
        this->push(-scale.x, -scale.y, -scale.z,  -1.0, -1.0, -1.0, color);
        this->push(scale.x, -scale.y, -scale.z,  1.0, -1.0, -1.0, color);
        this->push(-scale.x,  scale.y, -scale.z,  -1.0, 1.0, -1.0, color);
        this->push(-scale.x,  scale.y, -scale.z,  -1.0, 1.0, -1.0, color);
        this->push(scale.x, -scale.y, -scale.z,  1.0, -1.0, -1.0, color);
        this->push(scale.x,  scale.y, -scale.z,  1.0, 1.0, -1.0, color);

	
						//FRONT
        this->push(-scale.x, -scale.y,  scale.z,  -1.0, -1.0, 1.0, color);
        this->push(scale.x, -scale.y,  scale.z,  1.0, -1.0, 1.0, color);
        this->push(-scale.x,  scale.y,  scale.z,  -1.0, 1.0, 1.0, color);
        this->push(-scale.x,  scale.y,  scale.z,  -1.0, 1.0, 1.0, color);
        this->push(scale.x, -scale.y,  scale.z,  1.0, -1.0, 1.0, color);
        this->push(scale.x,  scale.y,  scale.z,  1.0, 1.0, 1.0, color);
}

void Cube::load(GLfloat scale) {
	this->load(glm::vec3(scale,scale,scale));    
}

void Cube::push(float x, float y, float z, float s, float t, float r, glm::vec4& color) {
	this->buffer.push_back(x);
	this->buffer.push_back(y);
	this->buffer.push_back(z);
	this->buffer.push_back(s);
	this->buffer.push_back(t);
	this->buffer.push_back(r);
	this->buffer.push_back(color.r);
	this->buffer.push_back(color.g);
	this->buffer.push_back(color.b);
	this->buffer.push_back(color.a);
}

void Cube::render() {


	
	//Disable all client states that could interfere with drawing the skybox.
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glPushMatrix();
	glMultMatrixf(glm::value_ptr(this->matrix));


	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );


	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, 10 * sizeof(GLfloat), &this->buffer[0]);
	glTexCoordPointer(3, GL_FLOAT, 10 * sizeof(GLfloat), &this->buffer[0] + 3);
	glColorPointer(4, GL_FLOAT, 10 * sizeof(GLfloat), &this->buffer[0]+6);
	

	glBindTexture(GL_TEXTURE_CUBE_MAP, this->textureID); //bind

	glEnable(GL_TEXTURE_CUBE_MAP);

	glEnable (GL_BLEND);

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDisable(GL_BLEND);

	glDisable(GL_TEXTURE_CUBE_MAP);

	glBindTexture(GL_TEXTURE_CUBE_MAP, NULL); //unbind

	glDisableClientState(GL_VERTEX_ARRAY); //disable these client states again after skybox is done with them.
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glPopMatrix();


}

void Cube::upload(sf::Image& front, sf::Image& back, 	sf::Image& left, sf::Image& right, sf::Image& up, sf::Image& down) {
	glEnable(GL_TEXTURE_CUBE_MAP);
	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &this->textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->textureID);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, front.getSize().x, front.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, front.getPixelsPtr());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, back.getSize().x, back.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, back.getPixelsPtr());

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, right.getSize().x, right.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, right.getPixelsPtr());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, left.getSize().x, left.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, left.getPixelsPtr());

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, up.getSize().x, up.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, up.getPixelsPtr());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, down.getSize().x, down.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, down.getPixelsPtr());

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, NULL);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_CUBE_MAP);

	this->uploaded = true;
}

Skybox::Skybox(bool load) {
	if(load) {
		this->load();
	}
}

void Skybox::load() {


		if(!this->front.loadFromFile(Assets::Path("Starfield/png/front.png"))
			|| !this->back.loadFromFile(Assets::Path("Starfield/png/back.png"))
			|| !this->left.loadFromFile(Assets::Path("../Assets/Starfield/png/left.png"))
			|| !this->right.loadFromFile(Assets::Path("../Assets/Starfield/png/right.png"))
			|| !this->up.loadFromFile(Assets::Path("../Assets/Starfield/png/up.png"))
			|| !this->down.loadFromFile(Assets::Path("../Assets/Starfield/png/down.png"))) {
				cout << ("[Skybox::load] Load image error.") << endl;
				Platform::Halt();
		}

		this->cube.upload(this->front,this->back,this->left,this->right,this->up,this->down);

		this->cube.load(float(ConfigurationFile::Client().getInt("view.size")));
}

void Skybox::render(sf::RenderWindow& window) {
	this->cube.matrix = glm::translate(glm::mat4(1.0f), this->position);
	this->cube.render();

}


