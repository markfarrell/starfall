//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).
#include "Starfall/Assets.h"

#include <Poco/Runnable.h>

using namespace Starfall;

AssetLoader Assets::Loader;

namespace Starfall {
	class AssetRunnable : public Poco::Runnable { //Loads an asset concurrently

		public:
			Asset::Ptr asset; 

			AssetRunnable(Asset::Ptr asset) {
				this->asset = asset;
			}

			void run() {
				this->asset->loaded = false;
				this->asset->load();
				this->asset->loaded = true;
			}

	};
}

Asset::Asset() : loaded(false) {}

bool Asset::isLoaded() {
	return this->loaded;
}

AssetLoader::AssetLoader() : pool(2, 16, 60, 1024) {} 


AssetLoader::~AssetLoader() { 
	this->pool.stopAll();
}

void AssetLoader::stopAll() {
	this->pool.stopAll();
}

void AssetLoader::enqueue(Asset::Ptr asset) {
	this->loadQueue.push_back(AssetRunnable(asset));
}

void AssetLoader::load() {
	for(vector<AssetRunnable>::iterator it = this->loadQueue.begin(); it != this->loadQueue.end(); it++) {
		this->pool.start((*it));
	}
}

bool AssetLoader::isLoaded() {
	for(vector<AssetRunnable>::iterator it = this->loadQueue.begin(); it != this->loadQueue.end(); it++) {
		if(!(*it).asset->isLoaded()) {
			return false;
		}
	}
	return true;
}

void AssetLoader::clear() {
	this->loadQueue.clear();
}
