//Copyright (c) 2013 Mark Farrell
#pragma once

#include "Starfall/Assets.h"
#include "Starfall/Technique.h"
#include "Starfall/Skybox.h"
#include "Starfall/Camera.h"

#include <Poco/Dynamic/Var.h>
#include <Poco/Dynamic/Struct.h>
#include <Poco/Foundation.h>
#include <Poco/SharedPtr.h>
#include <Poco/Mutex.h>
#include <Poco/Runnable.h>
#include <Poco/ScopedLock.h>

#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <map>
#include <string>


using std::map;
using std::vector;
using std::cout;
using std::endl;
using std::string;

namespace Starfall {


	class Material {
		public:
			typedef Poco::SharedPtr<Material> Ptr;
			string name;
			float alpha;
			glm::vec3 specularColor;
			glm::vec3 diffuseColor;
			Material() : alpha(0.0f), name("Material") {}
	};

	class Face {
		public:
			typedef Poco::SharedPtr<Face> Ptr;
			vector<glm::vec3> vertices;
			vector<glm::vec3> normals;
			Poco::UInt32 materialIndex;
			Face() : materialIndex(0) {}
	};

	class BoundingBox {
		public:
			typedef Poco::SharedPtr<BoundingBox> Ptr;
			glm::vec3 min;
			glm::vec3 max;
			glm::vec3 size();
	};


	class Mesh {
		public:
			typedef Poco::SharedPtr<Mesh> Ptr;
			string name;
			vector<Material::Ptr> materials; //materials are shared between instances of a mesh (new material pointers can be created for an instance of a mesh)
			vector<Face::Ptr> faces; //faces are shared between instances of a mesh
			BoundingBox::Ptr boundingBox;
			Mesh();
			Mesh(Mesh::Ptr& mesh);
	};


	/** Description: Stores OpenGL valid data for a mesh. Should be reinstantiated when the geometry of a mesh is updated. **/
	class MeshRenderer {

		private: 
			Mesh::Ptr mesh; 

		public:
			typedef Poco::SharedPtr<MeshRenderer> Ptr; //allocate renderers on heap
			vector<GLfloat> data; //stores vertex and color data
			GLsizei count; //number of elements in data array

			sf::Image cubeImage;
			Cube::Ptr cube;

			MeshRenderer(Mesh::Ptr& mesh); //populate data and count
			void update(Mesh::Ptr& mesh);

	};


	class Model : public Asset {
		public:
			typedef Poco::SharedPtr<Model> Ptr;

			static Model::Ptr Model::Create(string path);

			glm::mat4 matrix;
			glm::vec3 position; //the model's position should only be accessed in one thread
			glm::quat orientation; //likewise, the model's rotation should only be accessed in one thread

			BoundingBox::Ptr boundingBox; //bounding volume around the entire mesh.

			map<string, bool> states;

			Model(Model::Ptr pModel); //create model from existing model in memory
			~Model();

			virtual void run();

			virtual void load(); //load from path; thread-safe
			void update(); //recreate mesh renderers
			void render(Technique::Ptr& technique); //renders the model; thread safe operation

			void apply(Camera& camera); //recalculate the model's matrix after a change in position or rotation

	

		private:

	
			Model(string path); //load a .json 3D model from the given path

			Poco::Mutex mutex; //Model is loading in a separate thread. Is the model finished loading
	

			//Members are not thread-safe; do not allow direct access
			vector<MeshRenderer::Ptr> renderers; 
			vector<Mesh::Ptr> meshes;
			string path; //the model's path


			/**
			 *  Description: Rather than reloading a model each time from a file when it is instantiated, copy a vector that was already parsed. 
			 *  In C++ vectors are deep copied: each mesh will be new (on the heap because it's stored in a vector)
			 *  TODO: Mesh data and materials should be shared using Poco::Shared Pointers. Materials could be replaced with new shared pointers.
			 */
			static map<string, vector<Mesh::Ptr>> Resources; 
			static Poco::Mutex ResourcesMutex; //lock parallel access to Resources

			template<typename T> 
			T parse (string member, Poco::Dynamic::Struct<string>& fromStruct, bool debug=false);

			vector<Material::Ptr> parseMaterials(Poco::Dynamic::Struct<string>& meshStruct);
			vector<Face::Ptr> parseFaces(Poco::Dynamic::Struct<string>& meshStruct);
			BoundingBox::Ptr calculateBoundingBox(vector<Face::Ptr>& meshFaces);

			vector<Mesh::Ptr> parseMeshes(Poco::Dynamic::Var& jsonVar); 



	};

	template<typename T>
	inline T Model::parse(string member, Poco::Dynamic::Struct<string>& fromStruct, bool debug) {
		cout << "[Model::parse] InvalidAccessException: A parser has not been implemented for this type." << endl;
		throw Poco::InvalidAccessException();
	}

	template<>
	inline glm::vec3 Model::parse<glm::vec3>(string member, Poco::Dynamic::Struct<string>& fromStruct, bool debug) {
		if(fromStruct.contains(member)) {
			if(fromStruct[member].isArray()) {
				vector<Poco::Dynamic::Var> normal = fromStruct[member].extract<vector<Poco::Dynamic::Var>>(); 
				if(normal.size() == 3) {
					double x,y,z = 0.0f;
					Poco::NumberParser::tryParseFloat(normal[0].extract<string>(), x);
					Poco::NumberParser::tryParseFloat(normal[1].extract<string>(), y);
					Poco::NumberParser::tryParseFloat(normal[2].extract<string>(), z);
					if(debug) { cout << "[Model::parse<sf::Vector3f>] " << "(" << x << "," << y << "," << z << ")"<< endl; }
					return glm::vec3(float(x),float(y),float(z));
				}
			}
		}
		cout << "[Model::parse<sf::Vector3f>] Invalid Access Exception: Parsing Failed." << endl;
		throw Poco::InvalidAccessException();
	}

	template<>
	inline vector<glm::vec3> Model::parse<vector<glm::vec3>>(string member, Poco::Dynamic::Struct<string>& fromStruct, bool debug) {
		if(fromStruct.contains(member)) {
			if(fromStruct[member].isArray()) {
				vector<glm::vec3> parsedVector;
				vector<Poco::Dynamic::Var> elements = fromStruct[member].extract<vector<Poco::Dynamic::Var>>(); 
				for(vector<Poco::Dynamic::Var>::iterator elementsIterator = elements.begin(); elementsIterator != elements.end(); elementsIterator++) {
					if((*elementsIterator).isArray()) {
						vector<Poco::Dynamic::Var> element = (*elementsIterator).extract<vector<Poco::Dynamic::Var>>(); 
						if( element.size() == 3) {
							double x,y,z = 0.0f;
							Poco::NumberParser::tryParseFloat(element[0].extract<string>(), x);
							Poco::NumberParser::tryParseFloat(element[1].extract<string>(), y);
							Poco::NumberParser::tryParseFloat(element[2].extract<string>(), z);
							if(debug) { cout << "[Model::parse<vector<glm::vec3>>] glm::vec3" << "(" << x << "," << y << "," << z << ")"<< endl; }
							parsedVector.push_back(glm::vec3(float(x),float(y),float(z)));
						}
					}
				}
				return parsedVector;
			}
		} 
		cout << "[Model::parse<vector<glm::vec3>>] Invalid Access Exception: Parsing Failed." << endl;
		throw Poco::InvalidAccessException();
	}

	template<> 
	inline Poco::UInt32 Model::parse<Poco::UInt32>(string member, Poco::Dynamic::Struct<string>& fromStruct, bool debug) {
		if(fromStruct.contains(member)) {
			if(fromStruct[member].isString()) {
				int parsedInt = 0;
				Poco::NumberParser::tryParse(fromStruct[member].extract<string>(), parsedInt);
				if(debug) { cout << "[Model::parse<Poco::UInt32>] " << parsedInt << endl; }
				return Poco::UInt32(parsedInt);
			}
		}
		cout << "[Model::parse<Poco::UInt32>] Invalid Access Exception: Parsing Failed." << endl;
		throw Poco::InvalidAccessException();
	}

	template<> 
	inline float Model::parse<float>(string member, Poco::Dynamic::Struct<string>& fromStruct, bool debug) {
		if(fromStruct.contains(member)) {
			if(fromStruct[member].isString()) {
				double parsedFloat = 0.0f;
				Poco::NumberParser::tryParseFloat(fromStruct[member].extract<string>(), parsedFloat);
				if(debug) { cout << "[Model::parse<float>] " << parsedFloat << endl; }
				return float(parsedFloat);
			}
		}
		cout << "[Model::parse<Poco::float>] Invalid Access Exception: Parsing Failed." << endl;
		throw Poco::InvalidAccessException();
	}

	template<> 
	inline string Model::parse<string>(string member, Poco::Dynamic::Struct<string>& fromStruct, bool debug) {
		if(fromStruct.contains(member)) {
			if(fromStruct[member].isString()) {
				if(debug) { cout << "[Model::parse<string>] " << fromStruct[member].extract<string>() << endl; }
				return fromStruct[member].extract<string>();
			}
		}
		cout << "[Model::parse<string>] Invalid Access Exception: Parsing Failed." << endl;
		throw Poco::InvalidAccessException();
	}

	template<> 
	inline vector<Poco::Dynamic::Var> Model::parse<vector<Poco::Dynamic::Var>>(string member, Poco::Dynamic::Struct<string>& fromStruct, bool debug) {
		if(fromStruct.contains(member)) {
			if(fromStruct[member].isArray()) {
				if(debug) { cout << "[Model::parse<vector<Poco::Dynamic::Var>>] " << endl; }
				return fromStruct[member].extract<vector<Poco::Dynamic::Var>>();
			}
		}
		cout << "[Model::parse<vector<Poco::Dynamic::Var>>] Invalid Access Exception: Parsing Failed." << endl; 
		throw Poco::InvalidAccessException();
	}

}
