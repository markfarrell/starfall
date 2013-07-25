#pragma once

#include <string>
using std::string;

/** Used exclusively in this source file. */
class Strings {
	public:
		static bool EndsWith(const string& a, const string& b);
		static void ReplaceAll(string& str, string from, string to);
};
