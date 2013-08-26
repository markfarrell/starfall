//Copyright (c) 2013 Mark Farrell

#include "Starfall\ToonTechnique.h"


#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace Starfall;


void FillPass::beginPass() {


	glStencilFunc( GL_ALWAYS, 1, 0xFFFF );
	glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
	// Render the object in black
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}




void LinePass::beginPass() {


	glStencilFunc( GL_NOTEQUAL, 1, 0xFFFF );
	glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
	// Draw the object with thick lines
	glLineWidth( 1.5f );
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	if(!this->shader->isLoaded()) {
		cout << "Loading shader." << endl;
		this->shader->loaded = false;
		this->shader->load();
		this->shader->find("color");
		this->shader->loaded = true;
	} else {
		cout << "Not loading shader." << endl;
	}

	shader->use();
	shader->set<glm::vec4>("color", glm::vec4(0.0,0.0,0.0,1.0));
}

void LinePass::endPass() {
	shader->clear();
}


LinePass::LinePass(Shader::Ptr s) 
{
	this->shader = s;
}


ToonTechnique::ToonTechnique() : Technique() {
	this->shader = Shader::Ptr(new Shader("Shaders/Color.vert", "Shaders/Color.frag"));
}

Technique::Ptr ToonTechnique::Create() {
	Technique::Ptr toonTechnique(new ToonTechnique());
	Assets::Loader.enqueue(toonTechnique);
	return toonTechnique;
}

void ToonTechnique::load() {
	Poco::ScopedLock<Poco::Mutex> lock(this->mutex);

	this->passes.clear();
	this->passes.resize(2);
	this->passes[0] = (Pass::Ptr(new FillPass())); 
	this->passes[1] = (Pass::Ptr(new LinePass(shader))); //line pass loads on acquisition
}

void ToonTechnique::beginPasses() { 

	glPushAttrib( GL_ALL_ATTRIB_BITS );
	glEnable(GL_STENCIL_TEST);
	glClearStencil(0);
	glClear(GL_STENCIL_BUFFER_BIT);
}

void ToonTechnique::endPasses() {
	glDisable(GL_STENCIL_TEST);
	glPopAttrib();
}
