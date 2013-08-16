//Copyright (c) 2013 Mark Farrell

#include "Starfall\Camera.h"

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

using namespace Starfall;

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