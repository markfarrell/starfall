//Copyright (c) 2013 Mark Farrell
#pragma once


#include <SFML\Window.hpp>


namespace Starfall {

	class Application;

	/** 
	 * Description: Scene is an abstract class; when entered a scene will begin rendering and updating from SFML events passed to it.
	 * Children of scene must implement initialize(), render() and update(...);
	 */
	class Scene {
		protected:
			Application* parent;
			Scene* prevScene;
			Scene* nextScene;

			virtual void initialize()=0;

		public:
			void enter(Scene* prevScene=NULL, Scene* nextScene=NULL); //start rendering and updating this scene; set values for prev and next. calls initialize() to initialize the viewport for this scene.
			void prev(); //Enter the previous scene. If prev is NULL, the application will go back to the login scene
			void next(); //Enter the next scene. If next is NULL, the application will exit.

	
			virtual void render()=0;
			virtual void update()=0;
			virtual void load()=0;

			Scene(Application* parent); //set prev and next to NULL; the application manages instances of scenes
	};
}