//Copyright (c) 2013 Mark Farrell
#pragma once

#include <map>
#include <vector>
#include <string>

using std::string;

namespace Starfall {

	class ConfigurationFile {
			friend class Application;
			friend class Skybox;
		public:
			static ConfigurationFile Client(); //returns the client's main configuration file
			int getInt(string key, int default = 0);
			bool getBool(string key, bool default = false);
			string getString(string key, string default = "");
			bool initialized();
		private:
			ConfigurationFile();
			void initialize(string path);
			bool isInitialized;
			std::map<string, string> map; //key value pairs of config values
	};
}
