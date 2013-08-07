//Copyright (c) 2013 Mark Farrell
#pragma once
#include "Starfall\Platform.h"

using namespace Starfall;

std::ofstream Platform::out;

void Platform::Init() {
	Platform::out.open("client.log");
	cout.rdbuf(out.rdbuf());
#ifdef PLATFORM_IS_WINDOWS
	cout << "[Platform::Init] Starting on Windows." << endl;
#endif
}

void Platform::Halt() {
	cout << "[Platform::Halt] The Application has crashed.\n\nPlease contact: markfarr2011@gmail.com" << endl;
	if(Platform::out.is_open()) {
		Platform::out.close();
	}
	exit(1);
}