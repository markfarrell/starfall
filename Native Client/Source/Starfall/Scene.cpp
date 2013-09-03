//Copyright (c) 2013 Mark Farrell
#include "Starfall/Application.h"
#include "Starfall/Scene.h"

using namespace Starfall;

Scene::Scene(Application* parent) {
	this->parent = parent;
	this->prevScene = NULL;
	this->nextScene = NULL;
}

void Scene::enter(Scene* prevScene, Scene* nextScene) {
	this->prevScene = prevScene;
	this->nextScene = nextScene;
	this->initialize();
	this->parent->changeScene(this);
}

void Scene::next() { 
	if(this->nextScene != NULL) {
		this->nextScene->enter(this, NULL);
	} else {
		this->parent->logout();
	}
}

void Scene::prev() {
	if(this->prevScene != NULL) {
		this->prevScene->enter(NULL, this);
	} else {
		this->parent->logout();
	}
}