//Copyright (c) 2013 Mark Farrell



#pragma once
#include <string>
using std::string;

/* Change the defines for each type of build. */
#define PATH_DATA "../Data/"
#define PATH_PLUGINS "../Plugins/"

namespace Starfall {

	namespace Defines {

		class Path {
			public:
				/* Description: Returns folder as a string type. Using FOLDER would imply const char pointer array.
				  Usage: Defines::Path::Data()+"file.ext" */
				static string Data() { return PATH_DATA; } 
				static string Plugins() { return PATH_PLUGINS; } 
		};
	}

}