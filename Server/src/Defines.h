#pragma once
#include <string>
using std::string;

/* Change the defines for each type of build. */
#define PATH_DATA "../Data/"
#define PATH_WEB "../Web/"
#define PATH_SCRIPT "../Script/"

namespace Defines {

	class Path {
		public:
			/* Description: Returns folder as a string type. Using FOLDER would imply const char pointer array.
			  Usage: Defines::Path::Data()+"file.ext" */
			static string Data() { return PATH_DATA; } 
			static string Web() { return PATH_WEB; } /* Description: Path to static html */
			static string Script() { return PATH_SCRIPT; }
	};
}