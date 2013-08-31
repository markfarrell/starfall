//Copyright (c) 2013 Mark Farrell
#include "Starfall\Platform.h"
#include "Starfall\ConfigurationFile.h"
#include "Starfall\Assets.h"

#include <Poco\NumberParser.h>
#include <Poco\StringTokenizer.h>
#include <Poco\String.h>

#include <fstream>
#include <iostream>

using std::cout;
using std::endl;

using namespace Starfall;

ConfigurationFile ConfigurationFile::Client() {
	static ConfigurationFile clientConfig;
	if(!clientConfig.isInitialized) {
		std::vector<string> paths = Assets::ConfigPaths();
		for(std::vector<string>::iterator it = paths.begin(); it != paths.end(); it++) {
			if(!clientConfig.initialized()) {
				cout << "[ConfigurationFile::Client] Trying path " << (*it) << endl;
				clientConfig.initialize((*it));
			} else {
				cout << "[ConfigurationFile::Client] OK! " << (*it) << endl;
				break;
			}
		}

		if(!clientConfig.initialized()) {
			cout << "[ConfigurationFile::Client] Failed to load configuration file." << endl;
			Platform::Halt();
		}
	}
	return clientConfig;
}

ConfigurationFile::ConfigurationFile() {
	this->isInitialized = false;
}

void ConfigurationFile::initialize(string path) {
	if(!this->isInitialized) {
		std::ifstream configFile;
		configFile.open(path, std::ios::in | std::ios::binary);
		string line;
		if(configFile.is_open()) {
			while(configFile.good()) {
				std::getline(configFile,line);
				line = Poco::trim(line);
				Poco::StringTokenizer tokenizer(line, "=", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
				if(tokenizer.count() == 2) {
					this->map[tokenizer[0]] = tokenizer[1];
				} 
			}
			configFile.close();
			this->isInitialized = true;
		}
	}
}

bool ConfigurationFile::initialized() {
	return this->isInitialized;
}

int ConfigurationFile::getInt(std::string key, int default) {
	int ret = default;
	Poco::NumberParser::tryParse(this->map[key], ret);
	return ret;
}

bool ConfigurationFile::getBool(std::string key, bool default) {
	bool ret = default;
	if(this->map[key] == "true") {
		ret = true;
	} else if(this->map[key] == "false") {
		ret = false;
	}
	return ret;
}

string ConfigurationFile::getString(std::string key, string default) {
	string ret = default;
	if(this->map.find(key) != this->map.end()) {
		ret = this->map[key];
	}
	return ret;
}
