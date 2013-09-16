//Copyright (c) 2013 Mark Farrell
#pragma once

#include "Starfall/ConfigurationFile.h"

#include <Poco/Foundation.h>
#include <Poco/ThreadPool.h>
#include <Poco/HashMap.h>
#include <Poco/Mutex.h>
#include <Poco/ScopedLock.h>
#include <Poco/SharedPtr.h>

#include <string>
#include <vector>
#include <exception>
#include <queue>

using std::string;
using std::vector;
using std::exception;

namespace Starfall {

	class AssetRunnable;

	class Asset {
		friend class AssetRunnable;
		protected:
			Poco::Mutex mutex;
		public :
			typedef Poco::SharedPtr<Asset> Ptr;
			Asset();

			bool loaded;
			virtual void load()=0;
			virtual bool isLoaded();
	};

	class AssetLoader : public Asset {
		private:
			Poco::ThreadPool pool;
			vector<AssetRunnable> loadQueue;
		public:
			AssetLoader();
			~AssetLoader();
			void enqueue(Asset::Ptr asset);	
			void clear();
			virtual void load();
			virtual bool isLoaded(); //Returns true if all assets that were enqueued are loaded.
			void stopAll();
	};

	class Assets {

		public:

			static AssetLoader Loader;

			static string Path(string path) { return ConfigurationFile::Client().getString("assets.path")+path; } //declare inline
			static std::vector<string> ConfigPaths() { //description: This static inline method returns possible places to search for the config file needed to start the application.
				std::vector<string> paths;
				paths.push_back("./client.cfg");
				paths.push_back("../Assets/client.cfg");
				return paths;
			}
			
	};


}
