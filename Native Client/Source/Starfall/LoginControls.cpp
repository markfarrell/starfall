//Copyright (c) 2013 Mark Farrell
#include "Starfall\LoginControls.h"

#include "Starfall\LoginUI.h"

#include "Starfall\LoginStruct.h"
#include "Starfall\Client.h"

#include <Awesomium/WebCore.h>
#include <Awesomium/BitmapSurface.h>
#include <Awesomium/STLHelpers.h>
#include <Awesomium/DataSource.h>
#include <Awesomium/DataPak.h>
#include <Awesomium/WebViewListener.h>

#include <cmath>
#include <algorithm>

using namespace Starfall;

LoginControls::LoginControls(LoginUI* parent) :
	config(ConfigurationFile::Client()) 
{
	this->parent = parent;
	this->parent->view->CreateGlobalJavascriptObject(Awesomium::WSLit("login"));
	this->loginControlsObject = parent->view->CreateGlobalJavascriptObject(Awesomium::WSLit("login.controls")).ToObject();
	this->loginControlsObject.SetCustomMethod(Awesomium::WSLit("onConnect"), false);
	this->parent->view->set_js_method_handler(this);
}


void LoginControls::mouseMove(sf::Event& event) {
	sf::Vector2f moveVector = sf::Vector2f(float(event.mouseMove.x), float(event.mouseMove.y));
	if(this->parent->surfaceSprite.getGlobalBounds().contains(moveVector)) {
		sf::Vector2f distances;
		distances.x = sqrt(pow((moveVector.x-this->parent->surfaceSprite.getPosition().x),2));
		distances.y = sqrt(pow((moveVector.y-this->parent->surfaceSprite.getPosition().y),2));
		this->parent->view->InjectMouseMove(int(distances.x), int(distances.y));
	}
}

void LoginControls::mouseButtonPressed(sf::Event& event) {
	sf::Vector2f mouseVector = sf::Vector2f(float(event.mouseButton.x), float(event.mouseButton.y));
	if(this->parent->surfaceSprite.getGlobalBounds().contains(mouseVector)) {
		this->parent->view->Focus();
		if(event.mouseButton.button == sf::Mouse::Left) {
			this->parent->view->InjectMouseDown(Awesomium::kMouseButton_Left);
		} else if (event.mouseButton.button == sf::Mouse::Right) {
			this->parent->view->InjectMouseDown(Awesomium::kMouseButton_Right);
		} else if (event.mouseButton.button == sf::Mouse::Middle) {
			this->parent->view->InjectMouseDown(Awesomium::kMouseButton_Middle);
		}
	} else {
		this->parent->view->Unfocus();
	}
}

void LoginControls::mouseButtonReleased(sf::Event& event) {
	sf::Vector2f mouseVector = sf::Vector2f(float(event.mouseButton.x), float(event.mouseButton.y));
	if(this->parent->surfaceSprite.getGlobalBounds().contains(mouseVector)) {
		this->parent->view->Focus();
		if(event.mouseButton.button == sf::Mouse::Left) {
			this->parent->view->InjectMouseUp(Awesomium::kMouseButton_Left);
		} else if (event.mouseButton.button == sf::Mouse::Right) {
			this->parent->view->InjectMouseUp(Awesomium::kMouseButton_Right);
		} else if (event.mouseButton.button == sf::Mouse::Middle) {
			this->parent->view->InjectMouseUp(Awesomium::kMouseButton_Middle);
		}
	} else {
		this->parent->view->Unfocus();
	}
}

void LoginControls::textEvent(sf::Event& event) {
	if(this->parent->view->focused_element_type() != Awesomium::kFocusedElementType_None) { //if view is focussed inject key
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

		this->parent->view->InjectKeyboardEvent(keyEvent);
	}
}



void LoginControls::OnMethodCall(Awesomium::WebView* caller, unsigned int remote_object_id, const Awesomium::WebString& method_name, const Awesomium::JSArray& args) {
	if(remote_object_id == this->loginControlsObject.remote_id() &&
		method_name == Awesomium::WSLit("onConnect")) {
		if(args.size() == 2) { 
			if(Client::Get()->connect()) {

				LoginStruct loginStruct;

				unsigned int usernameLength = args[0].ToString().ToUTF8(NULL,0);
				unsigned int passwordLength = args[1].ToString().ToUTF8(NULL,0);
				loginStruct.username.resize(usernameLength);
				loginStruct.password.resize(passwordLength);

				args[0].ToString().ToUTF8(&loginStruct.username[0], usernameLength);
				args[1].ToString().ToUTF8(&loginStruct.password[0], passwordLength);

				Client::Get()->tryLogin(loginStruct);

			} else {
				this->parent->setStatus("Failed to connect.");
			}
		} else {
			cout << "[LoginUI::OnMethodCall method_name=\"onConnect\"]: Invalid number of arguments ("<<args.size()<<")" << endl;
		}
	}
}

Awesomium::JSValue LoginControls::OnMethodCallWithReturnValue(Awesomium::WebView* caller, unsigned int remote_object_id, const Awesomium::WebString& method_name, const Awesomium::JSArray& args) {
	return Awesomium::JSValue::Undefined();
}

int LoginControls::mapKey(sf::Keyboard::Key& sfmlKey) {
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


void LoginControls::keyEvent(Awesomium::WebKeyboardEvent::Type type, sf::Event& event) {
		if(this->parent->view->focused_element_type() != Awesomium::kFocusedElementType_None) { //if view is focussed inject key
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

			this->parent->view->InjectKeyboardEvent(keyEvent);
	}
}