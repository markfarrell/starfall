//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).
#include "Starfall/LoginUI.h"

#include "Starfall/LoginControls.h"

#include <cmath>
#include <algorithm>


using namespace Starfall;

Transition::Transition() { 
	this->active = false;
	this->resetTime = sf::Int32(1000);
	this->motionTime = sf::Int32(1000.0f/float(ConfigurationFile::Client().getInt("ui.maxFPS")));
}

void Transition::activate() {
	if(!this->isActive()) {
		this->active = true;
		this->resetClock.restart();
		this->motionClock.restart();
	}
}

bool Transition::isActive() {
	return this->active;
}

bool Transition::isDone() {
	if(this->resetClock.getElapsedTime().asMilliseconds() >= this->resetTime) { 
		this->active = false;
		return true;
	} 
	return false;
}

bool Transition::move() {
	if(this->isActive()) {
		if(this->motionClock.getElapsedTime().asMilliseconds() >= this->motionTime) { 
			this->motionClock.restart();
			return true;
		} 
	}
	return false;
}

float Transition::percentageSpeed() {
	return float(this->motionTime) / float(this->resetTime);
};


LoginUI::LoginUI() :
	config(ConfigurationFile::Client())
{

	this->maxFps = float(config.getInt("ui.maxFPS"));
	this->uiResetTime = 1000.0f/this->maxFps; //=40	

	Awesomium::WebConfig webConfig;
	webConfig.log_path = Awesomium::WSLit("./");
	this->core = Awesomium::WebCore::Initialize(webConfig);
	this->view = core->CreateWebView(config.getInt("ui.width"), config.getInt("ui.height"));

	this->dataSource = new Awesomium::DataPakSource(Awesomium::WSLit("../Assets/login.pak"));
	this->view->session()->AddDataSource(Awesomium::WSLit("Login"), dataSource);
	this->view->LoadURL(Awesomium::WebURL(Awesomium::WSLit("asset://Login/index.html")));
	this->view->SetTransparent(true);
	this->view->Focus();

	while(this->view->IsLoading()) {
		this->core->Update();
	}

	this->controls = new LoginControls(this);

}

	
LoginUI::~LoginUI() { 
	delete this->controls;
	this->view->Destroy();
	delete this->dataSource;
}


void LoginUI::center(sf::Vector2u windowSize) {
	this->surfaceSprite.setPosition(0.5f*sf::Vector2f(float(windowSize.x), float(windowSize.y))-0.5f*sf::Vector2f(float(surfaceSprite.getTextureRect().width), float(surfaceSprite.getTextureRect().height)));
}

bool LoginUI::contains(sf::Vector2f& mouseVector) {
	return this->surfaceSprite.getGlobalBounds().contains(mouseVector);
}


void LoginUI::updateSurface() {
	if(uiClock.getElapsedTime().asMilliseconds() >= uiResetTime) { 

		//Update UI status
		this->updateStatus();

		//Redraw surface
		Awesomium::BitmapSurface* surface = (Awesomium::BitmapSurface*)view->surface();
		surface->CopyTo((unsigned char*)surfaceImage.getPixelsPtr(), surface->width()*4, 4, true, false);
		surfaceTexture.update(surfaceImage);
		this->core->Update();


		uiClock.restart();
	}

	if(this->transition.move()) {
		sf::Uint8 subtractAlpha = sf::Uint8(this->transition.percentageSpeed()*255.0f);
		sf::Uint8 currentAlpha = this->surfaceSprite.getColor().a;
		if((int(currentAlpha)-int(subtractAlpha)) >= 0) { //Color stores values as bytes; Prevent the alpha from being reset to 255
			this->surfaceSprite.setColor(sf::Color(255,255,255,currentAlpha-subtractAlpha));
			this->surfaceSprite.move(-this->transition.percentageSpeed()*this->surfaceSprite.getGlobalBounds().width/2.0f, 0.0f);
		}
		
	}
}

void LoginUI::initSurface() {
	Awesomium::BitmapSurface* surface = (Awesomium::BitmapSurface*)view->surface();
	surfaceImage.create(surface->width(), surface->height());
	surface->CopyTo((unsigned char*)surfaceImage.getPixelsPtr(), surface->width()*4, 4, true, false);
	surfaceTexture.loadFromImage(surfaceImage);
	surfaceSprite = sf::Sprite(surfaceTexture);
}

void LoginUI::render(sf::RenderWindow& window) {
	window.pushGLStates();
	window.draw(surfaceSprite);
	window.popGLStates();
}

bool LoginUI::transitioned() {
	this->transition.activate();
	if(this->transition.isDone()) {
		return true;
	}
	return false;
}

void LoginUI::updateStatus() {
		this->statusMutex.lock();
		if(!this->statusStack.empty()) { 
			string status = statusStack.top();

			Awesomium::JSArray args;
			Awesomium::JSValue statusObject = this->view->ExecuteJavascriptWithResult(Awesomium::WSLit("statusMessage"), Awesomium::WSLit(""));
			args.Push(Awesomium::JSValue(Awesomium::WSLit(status.c_str())));
			statusObject.ToObject().Invoke(Awesomium::WSLit("update"), args);
		}
		this->statusMutex.unlock();
}

void LoginUI::setStatus(string status) {
	this->statusMutex.lock();
	while(!this->statusStack.empty()) {
		this->statusStack.pop(); //clear all elements from the stack; only store the most recent status
	}
	this->statusStack.push(status);
	this->statusMutex.unlock();
}