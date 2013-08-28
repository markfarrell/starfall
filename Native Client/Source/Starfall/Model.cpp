//Copyright (c) 2013 Mark Farrell
#include "Starfall/Model.h"
#include "Starfall/ConfigurationFile.h"
#include "Starfall/Assets.h"
#include "Starfall/Images.h"

#include <glm/gtx/norm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <iomanip>
#include <fstream>

using std::setw;

using namespace Starfall;


map<string, vector<Mesh::Ptr>> Model::Resources;
Poco::Mutex Model::ResourcesMutex;


glm::vec3 BoundingBox::size() {
	return this->max-this->min;
}


Mesh::Mesh() : name("Mesh")
{

}

Mesh::Mesh(Mesh::Ptr& mesh)
{
	this->name = mesh->name;
	this->boundingBox = mesh->boundingBox;
	this->faces = mesh->faces;
	this->materials = mesh->materials;
}

MeshRenderer::MeshRenderer(Mesh::Ptr& mesh) 
 : cube(Cube::Ptr(new Cube)) {


	if(!Images::Load(this->cubeImage, Assets::Path("Textures/box.png"))) {
		cout << "[MeshRenderer::MeshRender] Failed to load " << Assets::Path("Textures/box.png") << endl;
	}

	this->update(mesh);
}

void MeshRenderer::update(Mesh::Ptr& mesh) {

	this->mesh = mesh;

	this->count = 0;
	this->data.clear();

	this->cube->load(
		0.5f*glm::vec3(
			glm::abs(mesh->boundingBox->max.x-mesh->boundingBox->min.x),
			glm::abs(mesh->boundingBox->max.y-mesh->boundingBox->min.y),
			glm::abs(mesh->boundingBox->max.z-mesh->boundingBox->min.z)
		)
	); 

	this->cube->matrix = glm::translate(glm::mat4(1.0f), 0.5f*(mesh->boundingBox->max+mesh->boundingBox->min));


	for(vector<Face::Ptr>::iterator faceIterator = mesh->faces.begin(); faceIterator != mesh->faces.end(); faceIterator++) {

		vector<glm::vec3>& vertices = (*faceIterator)->vertices;
		vector<glm::vec3>& normals = (*faceIterator)->normals;
		Poco::UInt32& materialIndex = (*faceIterator )->materialIndex;

		if(materialIndex < mesh->materials.size()) { // material index is valid
			Material::Ptr pMaterial = mesh->materials[materialIndex];
			GLfloat r = GLfloat(pMaterial->diffuseColor.x);
			GLfloat g = GLfloat(pMaterial->diffuseColor.y);
			GLfloat b = GLfloat(pMaterial->diffuseColor.z);
			GLfloat a = GLfloat(pMaterial->alpha);


			if(vertices.size() == 3 &&
				normals.size() == 3) {
				for(Poco::UInt32 i = 0; i < 3; i++) {
					glm::vec3 vertex = vertices[i];
					glm::vec3 normal = normals[i];

					//Format: 3 vertex values, 3 color values per vertex
					this->data.push_back(GLfloat(vertex.x));
					this->data.push_back(GLfloat(vertex.y));
					this->data.push_back(GLfloat(vertex.z));
					this->data.push_back(GLfloat(normal.x));
					this->data.push_back(GLfloat(normal.y));
					this->data.push_back(GLfloat(normal.z));
					this->data.push_back(r);
					this->data.push_back(g);
					this->data.push_back(b);
					this->data.push_back(a);

					this->count++; //Increase total count on success
				}
			} else {
				cout << "[MeshRenderer::update] Invalid number of vertices on face " << vertices.size() << endl;
			}

		} else {
			cout << "[MeshRenderer::update] Invalid material index " << materialIndex << endl;
		}
	}
}

Model::Ptr Model::Create(string path) {
	Model::Ptr model(new Model(path));
	Assets::Loader.enqueue(model);
	return model;
}

Model::Model(string path)
	: Asset() {
	this->boundingBox = BoundingBox::Ptr(new BoundingBox);
	this->path = path;
	this->states["debug.boundingboxes"] = false;
}

Model::~Model() {}

void Model::run() { 
	this->load();
}

void Model::load() { //locks access to the model only when necessary; so that it's loading state can be checked by thread observers.

	bool foundResource = false;

	this->meshes.clear();


	/* lock(Model::Resources) */ { 
		Poco::ScopedLock<Poco::Mutex> resourcesLock(Model::ResourcesMutex);
		if(Model::Resources.find(path) != Model::Resources.end()) {
			Poco::ScopedLock<Poco::Mutex> lock(this->mutex);
			vector<Mesh::Ptr> resource =  Model::Resources[path];
			for(vector<Mesh::Ptr>::iterator it = resource.begin(); it != resource.end(); it++) {
				this->meshes.push_back(Mesh::Ptr(new Mesh(*it)));
			}
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

					vector<Mesh::Ptr> resource;
					for(vector<Mesh::Ptr>::iterator it = this->meshes.begin(); it != this->meshes.end(); it++) {
						resource.push_back(Mesh::Ptr(new Mesh(*it)));
					}
					Model::Resources[path] = resource; //copy this model's meshes to resources.
				}

			}
		}
	}

	this->update();  //load geometry into mesh renderers; thread-safe 
}


void Model::update() { //thread-safe
	Poco::ScopedLock<Poco::Mutex> lock(this->mutex);

	this->renderers.clear(); //release previous mesh renderers
	for(vector<Mesh::Ptr>::iterator meshIterator = this->meshes.begin(); meshIterator != this->meshes.end(); meshIterator++) { //map meshes to new mesh renderers
		this->renderers.push_back(MeshRenderer::Ptr(new MeshRenderer(*meshIterator)));

		this->boundingBox->min.x = glm::min((*meshIterator)->boundingBox->min.x, this->boundingBox->min.x);
		this->boundingBox->min.y = glm::min((*meshIterator)->boundingBox->min.y, this->boundingBox->min.y);
		this->boundingBox->min.z = glm::min((*meshIterator)->boundingBox->min.z, this->boundingBox->min.z);
		this->boundingBox->max.x = glm::max((*meshIterator)->boundingBox->max.x, this->boundingBox->max.x);
		this->boundingBox->max.y = glm::max((*meshIterator)->boundingBox->max.y, this->boundingBox->max.y);
		this->boundingBox->max.z = glm::max((*meshIterator)->boundingBox->max.z, this->boundingBox->max.z);

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
				material->diffuseColor = this->parse<glm::vec3>("diffuse_color", materialStruct);
				material->specularColor = this->parse<glm::vec3>("specular_color", materialStruct);

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
				face->vertices = this->parse<vector<glm::vec3>>("vertices", faceStruct);
				face->normals = this->parse<vector<glm::vec3>>("normals", faceStruct);
				face->materialIndex = this->parse<Poco::UInt32>("material_index", faceStruct);

				ret.push_back(face);
			}
		}
	}
	return ret;
}

vector<Mesh::Ptr> Model::parseMeshes(Poco::Dynamic::Var& jsonVar)
{

	
	if(jsonVar.isStruct()) {
		if(jsonVar["meshes"].isArray()) {
			vector<Poco::Dynamic::Var> asset = jsonVar["meshes"].extract<vector<Poco::Dynamic::Var>>(); //an asset is a collection (array) of objects, including meshes
			vector<Mesh::Ptr> meshes;
			for(vector<Poco::Dynamic::Var>::iterator assetIterator = asset.begin(); assetIterator != asset.end(); assetIterator++) {
				if((*assetIterator).isStruct()) {
					if((*assetIterator)["mesh"].isStruct()) {

						Poco::Dynamic::Struct<string> meshStruct = (*assetIterator)["mesh"].extract<Poco::Dynamic::Struct<string>>();
								
						Mesh::Ptr mesh(new Mesh);
						mesh->name = this->parse<string>("name", meshStruct);
						mesh->faces = this->parseFaces(meshStruct);
						mesh->materials = this->parseMaterials(meshStruct);
						mesh->boundingBox = this->calculateBoundingBox(mesh->faces);
						meshes.push_back(mesh);
					}
				}
			}
			return meshes;
		}
	} 
	throw Poco::InvalidAccessException();
}

BoundingBox::Ptr Model::calculateBoundingBox(vector<Face::Ptr>& meshFaces) {
	BoundingBox::Ptr boundingBox(new BoundingBox);

	bool initialized = false;
	float minX,maxX,minY, maxY, minZ, maxZ;
						
	for(vector<Face::Ptr>::iterator faceIterator = meshFaces.begin(); faceIterator != meshFaces.end(); faceIterator++) {
		vector<glm::vec3>& vertices = (*faceIterator)->vertices;
		for(vector<glm::vec3>::iterator verticesIterator = vertices.begin(); verticesIterator != vertices.end(); verticesIterator++) {

			glm::vec3& vertex = (*verticesIterator); 
			if(!initialized) {
				minX = vertex.x;
				maxX = vertex.x;
				minY = vertex.y;
				maxY = vertex.y;
				minZ = vertex.z;
				maxZ = vertex.z;
				initialized = true;
			} else {
				if(minX > vertex.x) {
					minX = vertex.x;
				} 

				if(maxX < vertex.x) {
					maxX = vertex.x;
				}

				if(minY > vertex.y) {
					minY = vertex.y;
				}

				if(maxY < vertex.y) {
					maxY = vertex.y;
				}

				if(minZ > vertex.z) { 
					minZ = vertex.z;
				}

				if(maxZ < vertex.z) {
					maxZ = vertex.z;
				}

					
			}
		}
	}

	boundingBox->min.x = minX;
	boundingBox->min.y = minY;
	boundingBox->min.z = minZ;
	boundingBox->max.x = maxX;
	boundingBox->max.y = maxY;
	boundingBox->max.z = maxZ;

	return boundingBox;
}

void Model::apply(Camera& camera) {
	glm::vec3 offset = -0.5f*glm::vec3(0.0f, this->boundingBox->size().y, 0.0f);
	this->position = camera.position + offset;
	this->orientation = glm::normalize(glm::quat(glm::inverse(camera.view))); 
	this->matrix = glm::mat4(1.0f);
	this->matrix = glm::translate(this->matrix, this->position-offset);
	this->matrix *= glm::toMat4(this->orientation);
	this->matrix = glm::translate(this->matrix, offset);
}


void Model::render(Technique::Ptr& technique) {

	glPushMatrix();
	glMultMatrixf(glm::value_ptr(this->matrix));

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);




	/** lock(this->mutex) **/ { 
		Poco::ScopedLock<Poco::Mutex> modelLock(this->mutex);
		for(vector<MeshRenderer::Ptr>::iterator rendererIterator = this->renderers.begin(); rendererIterator != this->renderers.end(); rendererIterator++) {

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);

			glVertexPointer(3, GL_FLOAT, 10 * sizeof(GLfloat), &(*rendererIterator)->data[0]);
			glNormalPointer(GL_FLOAT, 10 * sizeof(GLfloat), &(*rendererIterator)->data[0] + 3);
			glColorPointer(4, GL_FLOAT, 10 * sizeof(GLfloat), &(*rendererIterator)->data[0] + 6);

			/* lock(technique.mutex) */ {
				Poco::ScopedLock<Poco::Mutex> techniqueLock(technique->mutex);
				technique->beginPasses();

				for(vector<Pass::Ptr>::iterator it = technique->passes.begin(); it != technique->passes.end(); it++) {
					(*it)->beginPass();
					glDrawArrays(GL_TRIANGLES, 0, (*rendererIterator)->count);
					(*it)->endPass();
				}

				technique->endPasses();
			}

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);
		}

		if(this->states["debug.boundingboxes"]) { 
			for(vector<MeshRenderer::Ptr>::iterator rendererIterator = this->renderers.begin(); rendererIterator != this->renderers.end(); rendererIterator++) {
			
				if(!(*rendererIterator)->cube->uploaded) {
					(*rendererIterator)->cube->upload(
						(*rendererIterator)->cubeImage,
						(*rendererIterator)->cubeImage,
						(*rendererIterator)->cubeImage,
						(*rendererIterator)->cubeImage,
						(*rendererIterator)->cubeImage,
						(*rendererIterator)->cubeImage
					);
				}
			
				(*rendererIterator)->cube->render();
			}
		}
	}
	

	glPopMatrix();




}