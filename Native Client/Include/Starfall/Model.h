//Copyright (c) 2013 Mark Farrell
#pragma once

#include <Poco/Dynamic/Var.h>
#include <Poco/Dynamic/Struct.h>
#include <Poco/Foundation.h>
#include <Poco/SharedPtr.h>

#include <SFML/Graphics.hpp>

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
			sf::Vector3f specularColor;
			sf::Vector3f diffuseColor;
			Material() : alpha(0.0f), name("Material") {}
	};

	class Face {
		public:
			typedef Poco::SharedPtr<Face> Ptr;
			vector<sf::Vector3f> vertices;
			sf::Vector3f normal;
			Poco::UInt32 materialIndex;
			Face() : materialIndex(0) {}
	};


	class Mesh {
		public:
			string name;
			vector<Material::Ptr> materials; //materials are shared between instances of a mesh (new material pointers can be created for an instance of a mesh)
			vector<Face::Ptr> faces; //faces are shared between instances of a mesh
			Mesh() : name("Mesh") {}
	};

	class Model {
		public:
			
			vector<Mesh> meshes;
			Model(string path); //load a .json 3D model from the given path
			void render(); //renders the model.
		private:

			/**
			 *  Description: Rather than reloading a model each time from a file when it is instantiated, copy a vector that was already parsed. 
			 *  In C++ vectors are deep copied: each mesh will be new (on the heap because it's stored in a vector)
			 *  TODO: Mesh data and materials should be shared using Poco::Shared Pointers. Materials could be replaced with new shared pointers.
			 */
			static map<string, vector<Mesh>> Resources; 

			template<typename T> 
			T parse (string member, Poco::Dynamic::Struct<string>& fromStruct, bool debug=false);

			vector<Material::Ptr> parseMaterials(Poco::Dynamic::Struct<string>& meshStruct);
			vector<Face::Ptr> parseFaces(Poco::Dynamic::Struct<string>& meshStruct);
			vector<Mesh> parseMeshes(Poco::Dynamic::Var& jsonVar); 
	};

	template<typename T>
	inline T Model::parse(string member, Poco::Dynamic::Struct<string>& fromStruct, bool debug) {
		cout << "[Model::parse] InvalidAccessException: A parser has not been implemented for this type." << endl;
		throw Poco::InvalidAccessException();
	}

	template<>
	inline sf::Vector3f Model::parse<sf::Vector3f>(string member, Poco::Dynamic::Struct<string>& fromStruct, bool debug) {
		if(fromStruct.contains(member)) {
			if(fromStruct[member].isArray()) {
				vector<Poco::Dynamic::Var> normal = fromStruct[member].extract<vector<Poco::Dynamic::Var>>(); 
				if(normal.size() == 3) {
					double x,y,z = 0.0f;
					Poco::NumberParser::tryParseFloat(normal[0].extract<string>(), x);
					Poco::NumberParser::tryParseFloat(normal[1].extract<string>(), y);
					Poco::NumberParser::tryParseFloat(normal[2].extract<string>(), z);
					if(debug) { cout << "[Model::parse<sf::Vector3f>] " << "(" << x << "," << y << "," << z << ")"<< endl; }
					return sf::Vector3f(float(x),float(y),float(z));
				}
			}
		}
		cout << "[Model::parse<sf::Vector3f>] Invalid Access Exception: Parsing Failed." << endl;
		throw Poco::InvalidAccessException();
	}

	template<>
	inline vector<sf::Vector3f> Model::parse<vector<sf::Vector3f>>(string member, Poco::Dynamic::Struct<string>& fromStruct, bool debug) {
		if(fromStruct.contains(member)) {
			if(fromStruct[member].isArray()) {
				vector<sf::Vector3f> parsedVector;
				vector<Poco::Dynamic::Var> elements = fromStruct[member].extract<vector<Poco::Dynamic::Var>>(); 
				for(vector<Poco::Dynamic::Var>::iterator elementsIterator = elements.begin(); elementsIterator != elements.end(); elementsIterator++) {
					if((*elementsIterator).isArray()) {
						vector<Poco::Dynamic::Var> element = (*elementsIterator).extract<vector<Poco::Dynamic::Var>>(); 
						if( element.size() == 3) {
							double x,y,z = 0.0f;
							Poco::NumberParser::tryParseFloat(element[0].extract<string>(), x);
							Poco::NumberParser::tryParseFloat(element[1].extract<string>(), y);
							Poco::NumberParser::tryParseFloat(element[2].extract<string>(), z);
							if(debug) { cout << "[Model::parse<vector<sf::Vector3f>>] sf::Vector3f" << "(" << x << "," << y << "," << z << ")"<< endl; }
							parsedVector.push_back(sf::Vector3f(float(x),float(y),float(z)));
						}
					}
				}
				return parsedVector;
			}
		} 
		cout << "[Model::parse<vector<sf::Vector3f>>] Invalid Access Exception: Parsing Failed." << endl;
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
