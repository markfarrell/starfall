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
			int getInt(string key, int default=0);
			string getString(string key, string default="");
			bool initialized();
		private:
			ConfigurationFile();
			void initialize(string path);
			bool isInitialized;
			std::map<string, string> map; //key value pairs of config values
	};

	class Assets {
		public:
			static string Path(string path) { return ConfigurationFile::Client().getString("assets.path")+path; } //declare inline
			static std::vector<string> ConfigPaths() { //description: This static inline method returns possible places to search for the config file needed to start the application.
				std::vector<string> paths;
				paths.push_back("./client.cfg");
				paths.push_back("../Assets/client.cfg");
				return paths;
			}
	};

}