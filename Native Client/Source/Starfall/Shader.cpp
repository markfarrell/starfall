//Copyright (c) 2013 Mark Farrell
#include "Starfall\Shader.h"

#include "Starfall\ConfigurationFile.h"
#include "Starfall\Assets.h"

#include <fstream>
#include <iostream>

using std::cout;
using std::endl;

using namespace Starfall;

Shader::Shader(string v, string f)
	: Asset(),
	vertexShaderID(NULL), 
	fragmentShaderID(NULL),
	programID(NULL),
	vertexPath(v), 
	fragmentPath(f) 
{
}

Shader::~Shader() {
	Poco::ScopedLock<Poco::Mutex> lock(this->mutex);
	if(!this->programID != NULL) {
		glDetachShader(this->programID, this->vertexShaderID);
		glDetachShader(this->programID, this->fragmentShaderID);
	}

	if(this->vertexShaderID != NULL) { 
		glDeleteShader(this->vertexShaderID);
	}

	if(this->fragmentShaderID != NULL) {
		glDeleteShader(this->fragmentShaderID);
	}
	
	if(this->programID != NULL) { 
		glDeleteProgram(this->programID);
	}
}

void Shader::use() {
	Poco::ScopedLock<Poco::Mutex> lock(this->mutex);
	if(this->programID != NULL) {
		glUseProgram(this->programID);
	}
}

void Shader::clear() {
	glUseProgram(NULL);
}

void Shader::find(string name) {
	Poco::ScopedLock<Poco::Mutex> lock(this->mutex);
	if(this->programID != NULL) { 
		this->names[name] = glGetUniformLocation(this->programID, name.c_str());
	}
}


void Shader::load() {
	Poco::ScopedLock<Poco::Mutex> lock(this->mutex);
	this->vertexShaderID = this->createShader(GL_VERTEX_SHADER, this->vertexPath);
	this->fragmentShaderID = this->createShader(GL_FRAGMENT_SHADER, this->fragmentPath);
	this->programID = this->createProgram();
}


GLuint Shader::createShader(GLuint type, string path) { 
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
		if(result != GL_TRUE) 
		{
				string infoLog;
				int infoLogLength = 0;
				int charsWritten = 0;
				glGetShaderiv(this->vertexShaderID, GL_INFO_LOG_LENGTH,&infoLogLength);
				if (infoLogLength > 0)
				{
					infoLog.resize(infoLogLength);
					glGetShaderInfoLog(shaderID, infoLogLength, &charsWritten, &infoLog[0]);
					cout << "[Shader::createShader] " << infoLog << endl;
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

GLuint Shader::createProgram() { 
	GLuint id = glCreateProgram();
	glAttachShader(id, this->vertexShaderID);
	glAttachShader(id, this->fragmentShaderID);
	glLinkProgram(id);
	return id;
}



