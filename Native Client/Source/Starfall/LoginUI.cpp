//Copyright (c) 2013 Mark Farrell
#include "Starfall\LoginUI.h"

#include "Starfall\LoginControls.h"

#include <cmath>
#include <algorithm>


using namespace Starfall;


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
		Awesomium::BitmapSurface* surface = (Awesomium::BitmapSurface*)view->surface();
		surface->CopyTo((unsigned char*)surfaceImage.getPixelsPtr(), surface->width()*4, 4, true, false);
		surfaceTexture.update(surfaceImage);
		this->core->Update();
		uiClock.restart();
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

void LoginUI::setStatus(string status) {
	Awesomium::JSArray args;
	Awesomium::JSValue statusObject = this->view->ExecuteJavascriptWithResult(Awesomium::WSLit("statusMessage"), Awesomium::WSLit(""));
	args.Push(Awesomium::JSValue(Awesomium::WSLit(status.c_str())));
	statusObject.ToObject().Invoke(Awesomium::WSLit("update"), args);
}