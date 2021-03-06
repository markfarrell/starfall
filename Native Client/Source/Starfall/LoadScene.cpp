//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).
#include "Starfall/LoadScene.h"
#include "Starfall/Assets.h"
#include "Starfall/ConfigurationFile.h"
#include "Starfall/Application.h"
#include "Starfall/WorldScene.h"
#include "Starfall/Platform.h"

#include <iostream>
using std::cout;
using std::endl;

using namespace Starfall;

LoadScene::LoadScene(Application* parent)
	: Scene(parent)
{

}

void LoadScene::load() {
	if(!font.loadFromFile(Assets::Path("Fonts/arial.ttf"))) {
		cout << "[LoadScene::load] << Could not load file " << Assets::Path("Fonts/arial.ttf") << "!" << endl;
		Platform::Halt();
	}
	text.setFont(font);
	text.setColor(sf::Color(50,50,50,255));
	text.setString("Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).");
	text.setPosition(
		(float(this->parent->window.getSize().x)-text.getGlobalBounds().width) / 2.0f,
		(float(this->parent->window.getSize().y)-text.getGlobalBounds().height) / 2.0f
	);

}

void LoadScene::initialize() { 

}

void LoadScene::render() {
	this->parent->window.pushGLStates();
	this->parent->window.clear(sf::Color::White);
	this->parent->window.draw(text);
	this->parent->window.popGLStates();
}

void LoadScene::update() { 
	sf::Event event;
	while (this->parent->window.pollEvent(event))
	{
		// Close window: exit
		if (event.type == sf::Event::Closed) {
			this->parent->window.close();
		}
		// Resize event: adjust the viewport
		if (event.type == sf::Event::Resized) {
			text.setPosition(
				(float(this->parent->window.getSize().x)-text.getGlobalBounds().width) / 2.0f,
				(float(this->parent->window.getSize().y)-text.getGlobalBounds().height) / 2.0f
			);
			this->parent->window.setView(sf::View(sf::Vector2f(float(event.size.width/2), float(event.size.height/2)), sf::Vector2f(float(event.size.width), float(event.size.height))));
		}
	}

	if(Assets::Loader.isLoaded()) {
		this->nextScene->enter(NULL, this->parent->worldScene);
	}
}