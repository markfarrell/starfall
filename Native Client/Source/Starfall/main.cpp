//Copyright (c) 2013 Mark Farrell

#ifdef PLATFORM_IS_WINDOWS
  #include <Windows.h>
#endif

#include "Starfall/Platform.h"
#include "Starfall/Client.h"
#include "Starfall/Application.h"
#include "Starfall/Assets.h"
#include "Starfall/Model.h"
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

	Starfall::Client::Clear();

	Starfall::Platform::Halt(EXIT_SUCCESS);
}
