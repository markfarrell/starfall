//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).

#include "Starfall/ToonTechnique.h"


#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace Starfall;


FillPass::FillPass(bool& enabled) {
	this->enabled = enabled;
}

void FillPass::beginPass() {
	if(this->enabled) { 
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		glStencilFunc( GL_ALWAYS, 1, 1 );
		glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
	}
}

void FillPass::endPass() {

}



void LinePass::beginPass() {
	if(this->enabled) { 

		glStencilFunc( GL_NOTEQUAL, 1, 1 );
		glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
		// Draw the object with thick lines
		glLineWidth( 1.5f );
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

		if(!this->shader->isLoaded()) {
			this->shader->loaded = false;
			this->shader->load();
			this->shader->find("color");
			this->shader->loaded = true;
		} 

		shader->use();
		shader->set<glm::vec4>("color", glm::vec4(0.0,0.0,0.0,1.0));
	}
}

void LinePass::endPass() {
	if(this->enabled) { 
		shader->clear();
	}
}


LinePass::LinePass(Shader::Ptr s, bool& enabled) 
{
	this->shader = s;
	this->enabled = enabled;
}


ToonTechnique::ToonTechnique() : Technique() {
	this->enabled = ConfigurationFile::Client().getBool("stencil.enabled");
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
	this->passes[0] = (Pass::Ptr(new FillPass(this->enabled))); 
	this->passes[1] = (Pass::Ptr(new LinePass(shader, this->enabled))); //line pass loads on acquisition
}

void ToonTechnique::beginPasses() { 
	if(this->enabled) { 
		glPushAttrib( GL_ALL_ATTRIB_BITS );
		glClearStencil(0);
		glClear(GL_STENCIL_BUFFER_BIT);
		glEnable(GL_STENCIL_TEST);
	}
}

void ToonTechnique::endPasses() {
	if(this->enabled) {
		glDisable(GL_STENCIL_TEST);
		glPopAttrib();
	}
}
