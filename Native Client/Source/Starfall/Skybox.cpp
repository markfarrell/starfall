//Copyright (c) 2013 Mark Farrell
#include "Starfall\ConfigurationFile.h"
#include "Starfall\Platform.h"
#include "Starfall\Skybox.h"

#include <iostream>

using std::cout;
using std::endl;

using namespace Starfall;

void Cube::load(GLfloat scale) {
//LEFT
		this->push(-scale, -scale, -scale,  -1.0f, -1.0f, -1.0f);
		this->push(-scale,  scale, -scale,  -1.0f, 1.0f, -1.0f);
		this->push(-scale, -scale,  scale,  -1.0f, -1.0f, 1.0f);
		this->push(-scale, -scale,  scale,  -1.0f, -1.0f, 1.0f);
		this->push(-scale,  scale, -scale,  -1.0f, 1.0f, -1.0f);
        this->push(-scale,  scale,  scale,  -1.0f, 1.0f, 1.0f);
						//RIGHT
        this->push(scale, -scale, -scale,  1.0, -1.0, -1.0);
        this->push(scale,  scale, -scale,  1.0, 1.0, -1.0);
        this->push(scale, -scale,  scale,  1.0, -1.0, 1.0);
        this->push(scale, -scale,  scale,  0.0, -1.0, 1.0);
        this->push(scale,  scale, -scale,  1.0, 1.0, -1.0);
        this->push(scale,  scale,  scale,  1.0, 1.0, 1.0);
						//DOWN
        this->push(-scale, -scale, -scale,  -1.0, -1.0, -1.0);
        this->push(scale, -scale, -scale,  1.0, -1.0, -1.0);
        this->push(-scale, -scale,  scale,  -1.0, -1.0, 1.0);
        this->push(-scale, -scale,  scale,  -1.0, -1.0, 1.0);
        this->push(scale, -scale, -scale,  1.0, -1.0, -1.0);
        this->push(scale, -scale,  scale,  1.0, -1.0, 1.0);
						//UP
        this->push(-scale,  scale, -scale,  -1.0, 1.0, -1.0);
        this->push(scale,  scale, -scale,  1.0, 1.0, -1.0);
        this->push(-scale,  scale,  scale,  -1.0, 1.0, 1.0);
        this->push(-scale,  scale,  scale,  -1.0, 1.0, 1.0);
        this->push(scale,  scale, -scale,  1.0, 1.0, -1.0);
        this->push(scale,  scale,  scale,  1.0, 1.0, 1.0);
						//BACK
        this->push(-scale, -scale, -scale,  -1.0, -1.0, -1.0);
        this->push(scale, -scale, -scale,  1.0, -1.0, -1.0);
        this->push(-scale,  scale, -scale,  -1.0, 1.0, -1.0);
        this->push(-scale,  scale, -scale,  -1.0, 1.0, -1.0);
        this->push(scale, -scale, -scale,  1.0, -1.0, -1.0);
        this->push(scale,  scale, -scale,  1.0, 1.0, -1.0);
						//FRONT
        this->push(-scale, -scale,  scale,  -1.0, -1.0, 1.0);
        this->push(scale, -scale,  scale,  1.0, -1.0, 1.0);
        this->push(-scale,  scale,  scale,  -1.0, 1.0, 1.0);
        this->push(-scale,  scale,  scale,  -1.0, 1.0, 1.0);
        this->push(scale, -scale,  scale,  1.0, -1.0, 1.0);
        this->push(scale,  scale,  scale,  1.0, 1.0, 1.0);
}

void Cube::push(float x, float y, float z, float s, float t, float r) {
	this->buffer.push_back(x);
	this->buffer.push_back(y);
	this->buffer.push_back(z);
	this->buffer.push_back(s);
	this->buffer.push_back(t);
	this->buffer.push_back(r);
}

Skybox::Skybox(bool load) {
	if(load) {
		this->load();
	}
}

void Skybox::load() {
		this->textureID = NULL;

		if(!this->front.loadFromFile(Assets::Path("Starfield/png/front.png"))
			|| !this->back.loadFromFile(Assets::Path("Starfield/png/back.png"))
			|| !this->left.loadFromFile(Assets::Path("../Assets/Starfield/png/left.png"))
			|| !this->right.loadFromFile(Assets::Path("../Assets/Starfield/png/right.png"))
			|| !this->up.loadFromFile(Assets::Path("../Assets/Starfield/png/up.png"))
			|| !this->down.loadFromFile(Assets::Path("../Assets/Starfield/png/down.png"))) {
				cout << ("[Skybox::load] Load image error.") << endl;
				Platform::Halt();
		}

		glEnable(GL_TEXTURE_CUBE_MAP);

		glGenTextures(1, &this->textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, this->front.getSize().x, this->front.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->front.getPixelsPtr());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, this->back.getSize().x, this->back.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->back.getPixelsPtr());

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, this->right.getSize().x, this->right.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->right.getPixelsPtr());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, this->left.getSize().x, this->left.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->left.getPixelsPtr());

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, this->up.getSize().x, this->up.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->up.getPixelsPtr());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, this->down.getSize().x, this->down.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->down.getPixelsPtr());

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_CUBE_MAP, NULL);

		glDisable(GL_TEXTURE_CUBE_MAP);

		this->cube.load(float(ConfigurationFile::Client().getInt("view.size")));
}

void Skybox::render(sf::RenderWindow& window) {

	glPushMatrix();
	glTranslatef(this->position.x, this->position.y, this->position.z);

	glEnable(GL_TEXTURE_CUBE_MAP);
	//Disable all client states that could interfere with drawing the skybox.
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 6 * sizeof(GLfloat), &this->cube.buffer[0]);
	glTexCoordPointer(3, GL_FLOAT, 6 * sizeof(GLfloat), &this->cube.buffer[0] + 3);

	glBindTexture(GL_TEXTURE_CUBE_MAP, this->textureID); //bind
    // Draw the cube
    glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindTexture(GL_TEXTURE_CUBE_MAP, NULL); //unbind

	glDisableClientState(GL_VERTEX_ARRAY); //disable these client states again after skybox is done with them.
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glDisable(GL_TEXTURE_CUBE_MAP);

	glPopMatrix();
}


