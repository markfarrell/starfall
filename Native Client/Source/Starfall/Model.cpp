//Copyright (c) 2013 Mark Farrell
#include "Starfall/Model.h"
#include "Starfall/ConfigurationFile.h"

#include <fstream>

using namespace Starfall;

map<string, vector<Mesh>> Model::Resources;
Poco::Mutex Model::ResourcesMutex;

MeshRenderer::MeshRenderer(Mesh& mesh) {
	this->count = 0;
	for(vector<Face::Ptr>::iterator faceIterator = mesh.faces.begin(); faceIterator != mesh.faces.end(); faceIterator++) {
		vector<sf::Vector3f> vertices = (*faceIterator)->vertices;
		Poco::UInt32 materialIndex = (*faceIterator )->materialIndex;
		if(materialIndex < mesh.materials.size()) { // material index is valid
			Material::Ptr pMaterial = mesh.materials[materialIndex];
			GLfloat r = GLfloat(pMaterial->diffuseColor.x);
			GLfloat g = GLfloat(pMaterial->diffuseColor.y);
			GLfloat b = GLfloat(pMaterial->diffuseColor.z);


			if(vertices.size() == 3) {
				for(vector<sf::Vector3f>::iterator verticesIterator = vertices.begin(); verticesIterator  != vertices.end(); verticesIterator++) {
					sf::Vector3f vertex = (*verticesIterator);

					//Format: 3 vertex values, 3 color values per vertex
					this->data.push_back(GLfloat(vertex.x));
					this->data.push_back(GLfloat(vertex.y));
					this->data.push_back(GLfloat(vertex.z));
					this->data.push_back(r);
					this->data.push_back(g);
					this->data.push_back(b);

					this->count++; //Increase total count on success
				}
			} else {
				cout << "[MeshRenderer::MeshRenderer] Invalid number of vertices on face " << vertices.size() << endl;
			}

		} else {
			cout << "[MeshRenderer::MeshRenderer] Invalid material index " << materialIndex << endl;
		}
	}
}

Model::Model(string path) {
	this->loaded = false;
	this->path = path;
}

Model::~Model() {}

void Model::run() { 
	this->load();
}

void Model::load() { //locks access to the model only when necessary; so that it's loading state can be checked by thread observers.

	{ //Always set the loading state to false when loading begins; in case the model is reloaded
		Poco::ScopedLock<Poco::Mutex> lock(this->mutex);
		this->loaded = false;
	}


	bool foundResource = false;

	/* lock(Model::Resources) */ { 
		Poco::ScopedLock<Poco::Mutex> resourcesLock(Model::ResourcesMutex);
		if(Model::Resources.find(path) != Model::Resources.end()) {
			Poco::ScopedLock<Poco::Mutex> lock(this->mutex);
			this->meshes = Model::Resources[path];
			foundResource = true;
		} 
	}
		
	if(!foundResource) {

		std::ifstream modelFile;

		modelFile.open(Assets::Path(path));

		if(modelFile.is_open()) {

			string jsonData;
			modelFile.seekg(0, std::ios::end);
			jsonData.resize((unsigned int)(modelFile.tellg()));
			modelFile.seekg(0, std::ios::beg);
			modelFile.read(&jsonData[0], jsonData.size());

			/** lock(this->meshes) **/ { 
				Poco::ScopedLock<Poco::Mutex> lock(this->mutex);

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

				/* lock(Model::Resources) */ {
					Poco::ScopedLock<Poco::Mutex> resourcesLock(Model::ResourcesMutex);

					Model::Resources[path] = this->meshes; //copy this model's meshes to resources.
				}

			}
		}
	}

	{
		Poco::ScopedLock<Poco::Mutex> lock(this->mutex);
		this->loaded = true;
	}

	this->update();  //load geometry into mesh renderers; thread-safe 

}

bool Model::isLoaded() { 
	Poco::ScopedLock<Poco::Mutex> lock(this->mutex);
	return this->loaded;
}

void Model::update() { //thread-safe
	Poco::ScopedLock<Poco::Mutex> lock(this->mutex);

	this->renderers.clear(); //release previous mesh renderers
	for(vector<Mesh>::iterator meshIterator = this->meshes.begin(); meshIterator != this->meshes.end(); meshIterator++) { //map meshes to new mesh renderers
		this->renderers.push_back(MeshRenderer::Ptr(new MeshRenderer(*meshIterator)));
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

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);



	
	/** lock(this->mutex) **/ { 
		Poco::ScopedLock<Poco::Mutex> lock(this->mutex);
		for(vector<MeshRenderer::Ptr>::iterator rendererIterator = this->renderers.begin(); rendererIterator != this->renderers.end(); rendererIterator++) {

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);

			glVertexPointer(3, GL_FLOAT, 6 * sizeof(GLfloat), &(*rendererIterator)->data[0]);
			glColorPointer(3, GL_FLOAT, 6 * sizeof(GLfloat), &(*rendererIterator)->data[0] + 3);

			// Draw the cube
			glDrawArrays(GL_TRIANGLES, 0, (*rendererIterator)->count);


			glDisableClientState(GL_VERTEX_ARRAY); //disable these client states again after skybox is done with them.
			glDisableClientState(GL_COLOR_ARRAY);


		}
	}
	



}