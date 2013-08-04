//Copyright (c) 2013 Mark Farrell
#include "Starfall\Application.h"

using namespace Starfall;

Application::Application() {

	Awesomium::WebConfig config;
	config.log_path = Awesomium::WSLit("./");

	this->core = Awesomium::WebCore::Initialize(config);
	this->view = core->CreateWebView(512, 512);

	unsigned short numFiles;
	Awesomium::WriteDataPak(Awesomium::WSLit("../Assets/login.pak"), Awesomium::WSLit("../Assets/login"), Awesomium::WSLit(""), numFiles);

	this->dataSource = new Awesomium::DataPakSource(Awesomium::WSLit("../Assets/login.pak"));
	this->view->session()->AddDataSource(Awesomium::WSLit("Login"), dataSource);
	this->view->LoadURL(Awesomium::WebURL(Awesomium::WSLit("asset://Login/index.html")));
	this->view->SetTransparent(true);
	this->view->Focus();

	while(this->view->IsLoading()) {
		this->core->Update();
	}
}



Application::~Application() {
	this->view->Destroy();
	delete this->dataSource;
	Awesomium::WebCore::Shutdown();
}

void Application::run() {
	Awesomium::BitmapSurface* surface = (Awesomium::BitmapSurface*)view->surface();
	sf::Image surfaceImage;
	surfaceImage.create(surface->width(), surface->height());
	surface->CopyTo((unsigned char*)surfaceImage.getPixelsPtr(), 512*4, 4, true, false);
	//surfaceImage.createMaskFromColor(sf::Color::Magenta);
	sf::Texture surfaceTexture;
	surfaceTexture.loadFromImage(surfaceImage);
	sf::Sprite surfaceSprite(surfaceTexture);
	


    // Request a 32-bits depth buffer when creating the window
    sf::ContextSettings contextSettings;
    contextSettings.depthBits = 32;

	sf::Image icon; 
	if(!icon.loadFromFile("../Assets/icon.png")) {
		printf("Failed to load icon.");
		exit(1);
	}
    // Create the main window
	sf::RenderWindow window(sf::VideoMode(1024, 1024), "Client", sf::Style::Default, contextSettings);

	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    // Make it the active window for OpenGL calls
    window.setActive();

	surfaceSprite.setPosition(0.5f*sf::Vector2f(float(window.getSize().x), float(window.getSize().y))-0.5f*sf::Vector2f(float(surfaceSprite.getTextureRect().width), float(surfaceSprite.getTextureRect().height)));
	
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

    // Create a clock for measuring the time elapsed
    sf::Clock clock;
	sf::Clock uiClock;
	sf::Clock clickClock;
    // Start the game loop

	float maxFPS = 25.0f;
	float uiResetTime = 1000.0f/25.0f; //=40

	Starfall::Skybox skybox;

    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed) {
                window.close();
			}

			if(event.type == sf::Event::MouseMoved) {
				sf::Vector2f moveVector = sf::Vector2f(float(event.mouseMove.x), float(event.mouseMove.y));
				if(surfaceSprite.getGlobalBounds().contains(moveVector)) {
					sf::Vector2f distances;
					distances.x = sqrt(pow((moveVector.x-surfaceSprite.getPosition().x),2));
					distances.y = sqrt(pow((moveVector.y-surfaceSprite.getPosition().y),2));
					view->InjectMouseMove(int(distances.x), int(distances.y));
				}
			}

			if(event.type == sf::Event::MouseButtonPressed) {
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

			if(event.type == sf::Event::MouseButtonReleased) {
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

            // Escape key: exit
            if ((event.type == sf::Event::KeyPressed))
			{
				if(event.key.code == sf::Keyboard::Escape) {
					window.close();
				}
			}

            // Resize event: adjust the viewport
            if (event.type == sf::Event::Resized) {
                glViewport(0, 0, event.size.width, event.size.height);
				window.setView(sf::View(sf::Vector2f(float(event.size.width/2), float(event.size.height/2)), sf::Vector2f(float(event.size.width), float(event.size.height))));
				surfaceSprite.setPosition(0.5f*sf::Vector2f(float(window.getSize().x), float(window.getSize().y))-0.5f*sf::Vector2f(float(surfaceSprite.getTextureRect().width), float(surfaceSprite.getTextureRect().height)));
			}
        }

		if(uiClock.getElapsedTime().asMilliseconds() >= uiResetTime) { //TODO: Listen for javascript events and stop 
			surface = (Awesomium::BitmapSurface*)view->surface();
			surface->CopyTo((unsigned char*)surfaceImage.getPixelsPtr(), 512*4, 4, true, false);
			surfaceTexture.update(surfaceImage);
			this->core->Update();
			uiClock.restart();
		}

        // Clear the color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Apply some transformations to rotate the cube
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glRotatef(clock.getElapsedTime().asSeconds() * 1, 0.f, 0.f, 1.f);

		skybox.render();

		window.pushGLStates();
		window.draw(surfaceSprite);
		window.popGLStates();

        // Finally, display the rendered frame on screen
        window.display();
    }
}
