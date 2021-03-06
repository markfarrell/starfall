//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).
#pragma once
#include <Awesomium/WebCore.h>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include "Starfall/ConfigurationFile.h"
#include "Starfall/Assets.h"

namespace Starfall { 

	class LoginControls : public Awesomium::JSMethodHandler { 
		friend class Application;
		friend class LoginUI;
		friend class LoginScene;
		protected:

		LoginUI* parent; 

		ConfigurationFile config;

		Awesomium::JSObject loginControlsObject;

		virtual void OnMethodCall(Awesomium::WebView* caller, unsigned int remote_object_id, const Awesomium::WebString& method_name, const Awesomium::JSArray& args);
		virtual Awesomium::JSValue OnMethodCallWithReturnValue(Awesomium::WebView* caller, unsigned int remote_object_id, const Awesomium::WebString& method_name, const Awesomium::JSArray& args);

		void mouseMove(sf::Event& event);
		void mouseButtonPressed(sf::Event& event);
		void mouseButtonReleased(sf::Event& event);
			
		int mapKey(sf::Keyboard::Key &sfmlCode);
		void keyEvent(Awesomium::WebKeyboardEvent::Type type, sf::Event& event);
		void textEvent(sf::Event& event);

		LoginControls(LoginUI* parent);
	};
}