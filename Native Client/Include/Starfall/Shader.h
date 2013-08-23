//Copyright (c) 2013 Mark Farrell
#pragma once

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <exception>
#include <iostream>
#include <map>
#include <string>


using std::exception;
using std::cout;
using std::endl;
using std::map;
using std::string;



namespace Starfall {

	class ShaderException : public exception {
		private:
			string msg;
		public:
			ShaderException(const string m="Shader Exception: No implementation for this type.") : msg(m) { }
			const char* what() { return msg.c_str(); }
	};


	class Shader {
		private: 
			
			map<string, GLuint> names;

			string vertexPath;
			string fragmentPath;

			GLuint vertexShaderID;
			GLuint fragmentShaderID;
			GLuint programID;

			GLuint createShader(GLuint type, string path);
			GLuint createProgram();

		public:
			Shader(string vertexPath, string fragmentPath);
			~Shader();


			void load();
			void use(); //begin using the shader program
			void clear(); //stop using the shader program
			void find(string name); //get the uniform location of 'name' and store it in the Shader's 'names' map.

			template<typename T> 
			void set(string name, T& value);

	};

	template<typename T>
	inline void Shader::set (string name, T& value) {
		ShaderException shaderException;
		cout << "[Shader::set] " << shaderException.what() << endl;
		throw shaderException;
	}

	template <>
	inline void Shader::set<glm::mat4>(string name, glm::mat4& value) {
		glUniformMatrix4fv(this->names[name], 1, GL_FALSE, glm::value_ptr(value));
	}

	template <>
	inline void Shader::set<glm::vec3>(string name, glm::vec3& value) {
		glUniform3f(this->names[name], value.x, value.y, value.z);
	}

	template <>
	inline void Shader::set<glm::vec4>(string name, glm::vec4& value) {
		glUniform4f(this->names[name], value.x, value.y, value.z, value.w);
	}

}