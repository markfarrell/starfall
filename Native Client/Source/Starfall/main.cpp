//Copyright (c) 2013 Mark Farrell

#include "Starfall\Platform.h"
#include "Starfall\Client.h"
#include "Starfall\Application.h"
#include "Starfall\Assets.h"
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
	Starfall::Application application;	
	application.run();
	Starfall::Platform::Halt(EXIT_SUCCESS);
	return EXIT_SUCCESS;
}
