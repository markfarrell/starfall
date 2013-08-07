//Copyright (c) 2013 Mark Farrell
#pragma once

#include <fstream>
#include <iostream>
using std::cout;
using std::endl;

#if defined(_WIN32) || defined(__WIN32__)
	#define PLATFORM_IS_WINDOWS
#endif

#ifdef PLATFORM_IS_WINDOWS
	#define PLATFORM_ENTRY_POINT INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#endif

#ifndef PLATFORM_ENTRY_POINT
	#define PLATFORM_ENTRY_POINT int main ( void )
#endif

namespace Starfall {
	class Platform {
			static std::ofstream out;
		public:
			static void Init(); //description: redirect cout to a text file.
			static void Halt(); //description: log a message then close the application. Call this in case of a fatal error.
	};
}