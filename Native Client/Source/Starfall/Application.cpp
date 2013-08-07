//Copyright (c) 2013 Mark Farrell
#include "Starfall\Application.h"

#include <Windows.h>
#include <iostream>

using std::cout;
using std::endl;

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

	this->view->CreateGlobalJavascriptObject(Awesomium::WSLit("login"));
	this->loginControlsObject = this->view->CreateGlobalJavascriptObject(Awesomium::WSLit("login.controls")).ToObject();
	this->loginControlsObject.SetCustomMethod(Awesomium::WSLit("onConnect"), false);

	this->view->set_js_method_handler(this);
}


void LoginUI::center(sf::Vector2u windowSize) {
	this->surfaceSprite.setPosition(0.5f*sf::Vector2f(float(windowSize.x), float(windowSize.y))-0.5f*sf::Vector2f(float(surfaceSprite.getTextureRect().width), float(surfaceSprite.getTextureRect().height)));
}

bool LoginUI::contains(sf::Vector2f& mouseVector) {
	return this->surfaceSprite.getGlobalBounds().contains(mouseVector);
}

void LoginUI::mouseMove(sf::Event& event) {
	sf::Vector2f moveVector = sf::Vector2f(float(event.mouseMove.x), float(event.mouseMove.y));
	if(surfaceSprite.getGlobalBounds().contains(moveVector)) {
		sf::Vector2f distances;
		distances.x = sqrt(pow((moveVector.x-surfaceSprite.getPosition().x),2));
		distances.y = sqrt(pow((moveVector.y-surfaceSprite.getPosition().y),2));
		view->InjectMouseMove(int(distances.x), int(distances.y));
	}
}

void LoginUI::mouseButtonPressed(sf::Event& event) {
	sf::Vector2f mouseVector = sf::Vector2f(float(event.mouseButton.x), float(event.mouseButton.y));
	if(surfaceSprite.getGlobalBounds().contains(mouseVector)) {
		view->Focus();
		if(event.mouseButton.button == sf::Mouse::Left) {
			view->InjectMouseDown(Awesomium::kMouseButton_Left);
		} else if (event.mouseButton.button == sf::Mouse::Right) {
			view->InjectMouseDown(Awesomium::kMouseButton_Right);
		} else if (event.mouseButton.button == sf::Mouse::Middle) {
			view->InjectMouseDown(Awesomium::kMouseButton_Middle);
		}
	} else {
		view->Unfocus();
	}
}

void LoginUI::mouseButtonReleased(sf::Event& event) {
	sf::Vector2f mouseVector = sf::Vector2f(float(event.mouseButton.x), float(event.mouseButton.y));
	if(surfaceSprite.getGlobalBounds().contains(mouseVector)) {
		view->Focus();
		if(event.mouseButton.button == sf::Mouse::Left) {
			view->InjectMouseUp(Awesomium::kMouseButton_Left);
		} else if (event.mouseButton.button == sf::Mouse::Right) {
			view->InjectMouseUp(Awesomium::kMouseButton_Right);
		} else if (event.mouseButton.button == sf::Mouse::Middle) {
			view->InjectMouseUp(Awesomium::kMouseButton_Middle);
		}
	} else {
		view->Unfocus();
	}
}

void LoginUI::textEvent(sf::Event& event) {
	if(view->focused_element_type() != Awesomium::kFocusedElementType_None) { //if view is focussed inject key
		Awesomium::WebKeyboardEvent keyEvent;
		keyEvent.type = Awesomium::WebKeyboardEvent::kTypeChar;
		string text;
		text.resize(1); 
		sf::Utf<32>::encodeAnsi(event.text.unicode, text.begin());
		keyEvent.text[0] = text.c_str()[0];
		keyEvent.unmodified_text[0] = keyEvent.text[0];
		keyEvent.native_key_code = text.c_str()[0];
		keyEvent.virtual_key_code = text.c_str()[0];
		keyEvent.modifiers = 0;

		char* buf = new char[20];
		Awesomium::GetKeyIdentifierFromVirtualKeyCode(keyEvent.virtual_key_code, &buf);
		strcpy_s(keyEvent.key_identifier, 20, buf);
		delete buf;

		view->InjectKeyboardEvent(keyEvent);
	}
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

LoginUI::~LoginUI() { 
	this->view->Destroy();
	delete this->dataSource;
}

void LoginUI::OnMethodCall(Awesomium::WebView* caller, unsigned int remote_object_id, const Awesomium::WebString& method_name, const Awesomium::JSArray& args) {
	if(remote_object_id == this->loginControlsObject.remote_id() &&
		method_name == Awesomium::WSLit("onConnect")) {
		//TODO: Create a connection singleton object
		//if(socket.connect(sf::IpAddress(config.getString("server.address")), config.getInt("server.port"), sf::seconds(3)) != sf::Socket::Error) {
		//	unsigned char data [46] = {
		//		0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x1E, 0xFF, 0xFF, 0xFF, 0xFF,
		//   	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05,
		//		0x61, 0x64, 0x6D, 0x69, 0X6E, 0x00, 0x00, 0x00, 0x05, 0x61, 0x64, 0x6D, 0X69, 0X6E
		//	};
		//	socket.send(&data[0], 46);
		//}
		//socket.disconnect();
	}
}

Awesomium::JSValue LoginUI::OnMethodCallWithReturnValue(Awesomium::WebView* caller, unsigned int remote_object_id, const Awesomium::WebString& method_name, const Awesomium::JSArray& args) {
	 MessageBox(NULL, "MessageBox Text", "MessageBox caption", MB_OK);
	return Awesomium::JSValue::Undefined();
}

int LoginUI::mapKey(sf::Keyboard::Key& sfmlKey) {
	switch(sfmlKey) {
		case sf::Keyboard::A: return Awesomium::KeyCodes::AK_A;
		case sf::Keyboard::B: return Awesomium::KeyCodes::AK_B;
		case sf::Keyboard::C: return Awesomium::KeyCodes::AK_C;
		case sf::Keyboard::D: return Awesomium::KeyCodes::AK_D;
		case sf::Keyboard::E: return Awesomium::KeyCodes::AK_E;
		case sf::Keyboard::F: return Awesomium::KeyCodes::AK_F;
		case sf::Keyboard::G: return Awesomium::KeyCodes::AK_G;
		case sf::Keyboard::H: return Awesomium::KeyCodes::AK_H;
		case sf::Keyboard::I: return Awesomium::KeyCodes::AK_I;
		case sf::Keyboard::J: return Awesomium::KeyCodes::AK_J;
		case sf::Keyboard::K: return Awesomium::KeyCodes::AK_K;
		case sf::Keyboard::L: return Awesomium::KeyCodes::AK_L;
		case sf::Keyboard::M: return Awesomium::KeyCodes::AK_M;
		case sf::Keyboard::N: return Awesomium::KeyCodes::AK_N;
		case sf::Keyboard::O: return Awesomium::KeyCodes::AK_O;
		case sf::Keyboard::P: return Awesomium::KeyCodes::AK_P;
		case sf::Keyboard::Q: return Awesomium::KeyCodes::AK_Q;
		case sf::Keyboard::R: return Awesomium::KeyCodes::AK_R;
		case sf::Keyboard::S: return Awesomium::KeyCodes::AK_S;
		case sf::Keyboard::T: return Awesomium::KeyCodes::AK_T;
		case sf::Keyboard::U: return Awesomium::KeyCodes::AK_U;
		case sf::Keyboard::V: return Awesomium::KeyCodes::AK_V;
		case sf::Keyboard::W: return Awesomium::KeyCodes::AK_W;
		case sf::Keyboard::X: return Awesomium::KeyCodes::AK_X;
		case sf::Keyboard::Y: return Awesomium::KeyCodes::AK_Y;
		case sf::Keyboard::Z: return Awesomium::KeyCodes::AK_Z;
		case sf::Keyboard::Num0: return Awesomium::KeyCodes::AK_0;
		case sf::Keyboard::Num1: return Awesomium::KeyCodes::AK_1;
		case sf::Keyboard::Num2: return Awesomium::KeyCodes::AK_2;
		case sf::Keyboard::Num3: return Awesomium::KeyCodes::AK_3;
		case sf::Keyboard::Num4: return Awesomium::KeyCodes::AK_4;
		case sf::Keyboard::Num5: return Awesomium::KeyCodes::AK_5;
		case sf::Keyboard::Num6: return Awesomium::KeyCodes::AK_6;
		case sf::Keyboard::Num7: return Awesomium::KeyCodes::AK_7;
		case sf::Keyboard::Num8: return Awesomium::KeyCodes::AK_8;
		case sf::Keyboard::Num9: return Awesomium::KeyCodes::AK_9;
		case sf::Keyboard::Escape: return Awesomium::KeyCodes::AK_ESCAPE;
		case sf::Keyboard::LControl: return Awesomium::KeyCodes::AK_LCONTROL;
		case sf::Keyboard::LShift: return Awesomium::KeyCodes::AK_LSHIFT;
		case sf::Keyboard::LAlt: return Awesomium::KeyCodes::AK_LMENU;
		case sf::Keyboard::LSystem: return Awesomium::KeyCodes::AK_LWIN;
		case sf::Keyboard::RControl: return Awesomium::KeyCodes::AK_RCONTROL;
		case sf::Keyboard::RShift: return Awesomium::KeyCodes::AK_RSHIFT;
		case sf::Keyboard::RAlt: return Awesomium::KeyCodes::AK_RMENU;
		case sf::Keyboard::RSystem: return Awesomium::KeyCodes::AK_RWIN;
		case sf::Keyboard::LBracket: return Awesomium::KeyCodes::AK_OEM_4;
		case sf::Keyboard::RBracket: return Awesomium::KeyCodes::AK_OEM_6;
		case sf::Keyboard::SemiColon: return Awesomium::KeyCodes::AK_OEM_1;
		case sf::Keyboard::Comma: return Awesomium::KeyCodes::AK_OEM_COMMA;
		case sf::Keyboard::Period: return Awesomium::KeyCodes::AK_OEM_PERIOD;
		case sf::Keyboard::Quote: return Awesomium::KeyCodes::AK_OEM_7;
		case sf::Keyboard::Slash: return Awesomium::KeyCodes::AK_OEM_2;
		case sf::Keyboard::BackSlash: return Awesomium::KeyCodes::AK_OEM_5;
		case sf::Keyboard::Tilde: return Awesomium::KeyCodes::AK_OEM_3; //backquote `; tilde: ~
		case sf::Keyboard::Equal: return Awesomium::KeyCodes::AK_OEM_PLUS;
		case sf::Keyboard::Dash: return Awesomium::KeyCodes::AK_OEM_MINUS;
		case sf::Keyboard::Space: return Awesomium::KeyCodes::AK_SPACE;
		case sf::Keyboard::Return: return Awesomium::KeyCodes::AK_RETURN;
		case sf::Keyboard::BackSpace: return Awesomium::KeyCodes::AK_BACK;
		case sf::Keyboard::Tab: return Awesomium::KeyCodes::AK_TAB;
		case sf::Keyboard::PageUp: return Awesomium::KeyCodes::AK_PRIOR;
		case sf::Keyboard::PageDown: return Awesomium::KeyCodes::AK_NEXT;
		case sf::Keyboard::End: return Awesomium::KeyCodes::AK_END;
		case sf::Keyboard::Home: return Awesomium::KeyCodes::AK_HOME;
		case sf::Keyboard::Insert: return Awesomium::KeyCodes::AK_INSERT;
		case sf::Keyboard::Delete: return Awesomium::KeyCodes::AK_DELETE;
		case sf::Keyboard::Add: return Awesomium::KeyCodes::AK_ADD;
		case sf::Keyboard::Multiply: return Awesomium::KeyCodes::AK_MULTIPLY;
		case sf::Keyboard::Subtract: return Awesomium::KeyCodes::AK_SUBTRACT;
		case sf::Keyboard::Divide: return Awesomium::KeyCodes::AK_DIVIDE;
		case sf::Keyboard::Left: return Awesomium::KeyCodes::AK_LEFT;
		case sf::Keyboard::Right: return Awesomium::KeyCodes::AK_RIGHT;
		case sf::Keyboard::Up: return Awesomium::KeyCodes::AK_UP;
		case sf::Keyboard::Down: return Awesomium::KeyCodes::AK_DOWN;
		case sf::Keyboard::Numpad0: return Awesomium::KeyCodes::AK_NUMPAD0;
		case sf::Keyboard::Numpad1: return Awesomium::KeyCodes::AK_NUMPAD1;
		case sf::Keyboard::Numpad2: return Awesomium::KeyCodes::AK_NUMPAD2;
		case sf::Keyboard::Numpad3: return Awesomium::KeyCodes::AK_NUMPAD3;
		case sf::Keyboard::Numpad4: return Awesomium::KeyCodes::AK_NUMPAD4;
		case sf::Keyboard::Numpad5: return Awesomium::KeyCodes::AK_NUMPAD5;
		case sf::Keyboard::Numpad6: return Awesomium::KeyCodes::AK_NUMPAD6;
		case sf::Keyboard::Numpad7: return Awesomium::KeyCodes::AK_NUMPAD7;
		case sf::Keyboard::Numpad8: return Awesomium::KeyCodes::AK_NUMPAD8;
		case sf::Keyboard::Numpad9: return Awesomium::KeyCodes::AK_NUMPAD9;
		case sf::Keyboard::F1: return Awesomium::KeyCodes::AK_F1;
		case sf::Keyboard::F2: return Awesomium::KeyCodes::AK_F2;
		case sf::Keyboard::F3: return Awesomium::KeyCodes::AK_F3;
		case sf::Keyboard::F4: return Awesomium::KeyCodes::AK_F4;
		case sf::Keyboard::F5: return Awesomium::KeyCodes::AK_F5;
		case sf::Keyboard::F6: return Awesomium::KeyCodes::AK_F6;
		case sf::Keyboard::F7: return Awesomium::KeyCodes::AK_F7;
		case sf::Keyboard::F8: return Awesomium::KeyCodes::AK_F8;
		case sf::Keyboard::F9: return Awesomium::KeyCodes::AK_F9;
		case sf::Keyboard::F10: return Awesomium::KeyCodes::AK_F10;
		case sf::Keyboard::F11: return Awesomium::KeyCodes::AK_F11;
		case sf::Keyboard::F12: return Awesomium::KeyCodes::AK_F12;
		case sf::Keyboard::F13: return Awesomium::KeyCodes::AK_F13;
		case sf::Keyboard::F14: return Awesomium::KeyCodes::AK_F14;
		case sf::Keyboard::F15: return Awesomium::KeyCodes::AK_F15;
		case sf::Keyboard::Pause: return Awesomium::KeyCodes::AK_PAUSE;
	}
	return Awesomium::KeyCodes::AK_UNKNOWN;
}


void LoginUI::keyEvent(Awesomium::WebKeyboardEvent::Type type, sf::Event& event) {
		if(view->focused_element_type() != Awesomium::kFocusedElementType_None) { //if view is focussed inject key
			Awesomium::WebKeyboardEvent keyEvent;
			keyEvent.type = Awesomium::WebKeyboardEvent::kTypeKeyDown;

			keyEvent.native_key_code = this->mapKey(event.key.code);
			keyEvent.virtual_key_code = this->mapKey(event.key.code);

			keyEvent.modifiers = 0;

			if(event.key.control) {
				keyEvent.modifiers |= Awesomium::WebKeyboardEvent::kModControlKey;
			} 
			if(event.key.alt) {
				keyEvent.modifiers |= Awesomium::WebKeyboardEvent::kModAltKey;
			}
			if(event.key.shift) {
				keyEvent.modifiers |= Awesomium::WebKeyboardEvent::kModShiftKey;
			}
			if(event.key.system) {
				keyEvent.modifiers |= Awesomium::WebKeyboardEvent::kModMetaKey;
			}

			char* buf = new char[20];
			Awesomium::GetKeyIdentifierFromVirtualKeyCode(keyEvent.virtual_key_code, &buf);
			strcpy_s(keyEvent.key_identifier, 20, buf);
			delete buf;

			view->InjectKeyboardEvent(keyEvent);
	}
}

Application::Application() : 
	config(ConfigurationFile::Client()),
	skybox(false),
	window(sf::VideoMode(config.getInt("window.width"), config.getInt("window.height")), config.getString("window.title", "Fail"), sf::Style::Default, sf::ContextSettings(32)) // Create the main window
{

	unsigned short numFiles;
	Awesomium::WriteDataPak(Awesomium::WSLit("../Assets/login.pak"), Awesomium::WSLit("../Assets/login"), Awesomium::WSLit(""), numFiles);

	this->pLoginUI = new LoginUI();
}


Application::~Application() {
	delete this->pLoginUI;
	Awesomium::WebCore::Shutdown();
}

void Application::update() {
	// Process events
	sf::Event event;
	while (window.pollEvent(event))
	{
		// Close window: exit
		if (event.type == sf::Event::Closed) {
			window.close();
		}

		if(event.type == sf::Event::MouseMoved) {
			this->pLoginUI->mouseMove(event);
		}

		if(event.type == sf::Event::MouseButtonPressed) {
			this->pLoginUI->mouseButtonPressed(event);
		}

		if(event.type == sf::Event::MouseButtonReleased) {
			this->pLoginUI->mouseButtonReleased(event);
		}

		// Escape key: exit
		if ((event.type == sf::Event::KeyPressed))
		{
			if(event.key.code == sf::Keyboard::Escape) {
				window.close();
			} 
			this->pLoginUI->keyEvent(Awesomium::WebKeyboardEvent::kTypeKeyDown, event);
		}

		if ((event.type == sf::Event::KeyReleased))
		{
			this->pLoginUI->keyEvent(Awesomium::WebKeyboardEvent::kTypeKeyUp, event);
		}

		if(event.type == sf::Event::TextEntered) {
			this->pLoginUI->textEvent(event);
		}

		// Resize event: adjust the viewport
		if (event.type == sf::Event::Resized) {
			glViewport(0, 0, event.size.width, event.size.height);
			window.setView(sf::View(sf::Vector2f(float(event.size.width/2), float(event.size.height/2)), sf::Vector2f(float(event.size.width), float(event.size.height))));
			this->pLoginUI->center(window.getSize());
		}

	}

	this->pLoginUI->updateSurface();
}

void Application::render() {
	    // Clear the color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Apply some transformations to rotate the cube
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glRotatef(clock.getElapsedTime().asSeconds() * 1, 0.f, 0.f, 1.f);

		skybox.render(window);
		this->pLoginUI->render(window);


        // Finally, display the rendered frame on screen
        window.display();
}




void Application::run() {

	this->pLoginUI->initSurface();
	
	sf::Image icon; 
	if(!icon.loadFromFile("../Assets/icon.png")) {
		printf("Failed to load icon.");
		exit(1);
	}

	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    // Make it the active window for OpenGL calls
    window.setActive();

	this->pLoginUI->center(window.getSize());

	if(GLEW_OK != glewInit()) {
		printf("Glew failed to initialized.");
	}
    // Set the color and depth clear values
    glClearDepth(1.f);
    glClearColor(0.f, 0.f, 0.f, 1.f);

    // Enable Z-buffer read and write
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    // Disable lighting and texturing
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    // Configure the viewport (the same size as the window)
    glViewport(0, 0, window.getSize().x, window.getSize().y);

    // Setup a perspective projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat ratio = static_cast<float>(window.getSize().x) / window.getSize().y;
    glFrustum(-ratio, ratio, -1.f, 1.f, 1.f, 866.f); //far is diagonal distance across skybox-cube (distance from 0,0,0 to oppose edge)

	this->skybox.load();

    while (window.isOpen())
    {
		this->update();
		this->render();
    }
}
