//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).

#pragma once

#include <string>
using std::string;

namespace Starfall {

	/** Used exclusively in this source file. */
	class Strings {
		public:
			static bool EndsWith(const string& a, const string& b);
			static void ReplaceAll(string& str, string from, string to);
	};

}
