//Copyright (c) 2013 Mark Farrell
#include "Starfall/Model.h"
#include "Starfall/ConfigurationFile.h"


#include <fstream>

using namespace Starfall;

map<string, vector<Mesh>> Model::Resources;

Model::Model(string path) {

	if(Model::Resources.find(path) != Model::Resources.end()) {
		this->meshes = Model::Resources[path];
	} else {
		std::ifstream modelFile;

		modelFile.open(Assets::Path(path));

		if(modelFile.is_open()) {

			string jsonData;
			modelFile.seekg(0, std::ios::end);
			jsonData.resize((unsigned int)(modelFile.tellg()));
			modelFile.seekg(0, std::ios::beg);
			modelFile.read(&jsonData[0], jsonData.size());

			try {
				this->meshes = this->parseMeshes(Poco::Dynamic::Var::parse(jsonData));
			} catch (Poco::InvalidAccessException e) {
				cout << "[Model::load] " << e.what() << "." << endl;
			} catch (Poco::Exception e) {
				cout << "[Model::load] " << e.what() << "." << endl;
			} 

			if(this->meshes.size() == 0) {
				cout << "[Model::load] (Warning - No Mesh Data): " <<  jsonData <<  endl;
			}

			modelFile.close();
			Model::Resources[path] = this->meshes; //copy this model's meshes to resources.
		}
	}
}

vector<Material::Ptr> Model::parseMaterials(Poco::Dynamic::Struct<string>& meshStruct) {
	vector<Material::Ptr> ret; 
	vector<Poco::Dynamic::Var> materials = this->parse<vector<Poco::Dynamic::Var>>("materials", meshStruct);
	for(vector<Poco::Dynamic::Var>::iterator materialsIterator = materials.begin(); materialsIterator != materials.end(); materialsIterator++) {
		if((*materialsIterator).isStruct()) {
			if((*materialsIterator)["material"].isStruct()) {
				Poco::Dynamic::Struct<string> materialStruct = (*materialsIterator)["material"].extract<Poco::Dynamic::Struct<string>>();

				Material::Ptr material(new Material);
				material->alpha = this->parse<float>("alpha", materialStruct);
				material->name = this->parse<string>("name", materialStruct);
				material->diffuseColor = this->parse<sf::Vector3f>("diffuse_color", materialStruct);
				material->specularColor = this->parse<sf::Vector3f>("specular_color", materialStruct);

				ret.push_back(material);
			}
		}
	}
	return ret;
}

vector<Face::Ptr> Model::parseFaces(Poco::Dynamic::Struct<string>& meshStruct) {
	vector<Face::Ptr> ret;
	vector<Poco::Dynamic::Var> faces = this->parse<vector<Poco::Dynamic::Var>>("faces", meshStruct);
	for(vector<Poco::Dynamic::Var>::iterator facesIterator = faces.begin(); facesIterator != faces.end(); facesIterator++) {
		if((*facesIterator).isStruct()) {
			if((*facesIterator)["face"].isStruct()) {
				Poco::Dynamic::Struct<string> faceStruct = (*facesIterator)["face"].extract<Poco::Dynamic::Struct<string>>();

				Face::Ptr face(new Face);
				face->vertices = this->parse<vector<sf::Vector3f>>("vertices", faceStruct);
				face->normal = this->parse<sf::Vector3f>("normal", faceStruct);
				face->materialIndex = this->parse<Poco::UInt32>("material_index", faceStruct);

				ret.push_back(face);
			}
		}
	}
	return ret;
}

vector<Mesh> Model::parseMeshes(Poco::Dynamic::Var& jsonVar)
{
	if(jsonVar.isStruct()) {
		if(jsonVar["meshes"].isArray()) {
			vector<Poco::Dynamic::Var> asset = jsonVar["meshes"].extract<vector<Poco::Dynamic::Var>>(); //an asset is a collection (array) of objects, including meshes
			vector<Mesh> meshes;
			for(vector<Poco::Dynamic::Var>::iterator assetIterator = asset.begin(); assetIterator != asset.end(); assetIterator++) {
				if((*assetIterator).isStruct()) {
					if((*assetIterator)["mesh"].isStruct()) {

						Poco::Dynamic::Struct<string> meshStruct = (*assetIterator)["mesh"].extract<Poco::Dynamic::Struct<string>>();
								
						Mesh mesh;
						mesh.name = this->parse<string>("name", meshStruct);
						mesh.faces = this->parseFaces(meshStruct);
						mesh.materials = this->parseMaterials(meshStruct);
						meshes.push_back(mesh);
					}
				}
			}
			return meshes;
		}
	} 
	throw Poco::InvalidAccessException();
}

void Model::render() {
 //TODO: glVertexPointer(...) and glColorPointer(...)
}