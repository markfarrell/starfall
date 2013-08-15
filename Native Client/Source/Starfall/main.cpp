//Copyright (c) 2013 Mark Farrell

#include "Starfall\Platform.h"
#include "Starfall\Client.h"
#include "Starfall\Application.h"
#include "Starfall\Model.h"
#include <iostream>



////////////////////////////////////////////////////////////
/// Entry point of application
///
/// \return Application exit code
///
////////////////////////////////////////////////////////////
PLATFORM_ENTRY_POINT
{
	Starfall::Platform::Init();
	Starfall::Model("Models/Cube.json");
	Starfall::Application application;	
	application.run();
    return EXIT_SUCCESS;
}
