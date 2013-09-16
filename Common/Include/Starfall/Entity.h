//Copyright (c) 2013 Mark Farrell
#pragma once

#include <Poco/Net/Net.h>
#include <Poco/SharedPtr.h>
#include <Poco/Mutex.h>
#include <Poco/HashMap.h>

#include <v8.h>

#include <exception>
#include <vector>
#include <string>
#include <map>

#include "Starfall/Config.h"
#include "Starfall/Transform.h"
#include "Starfall/TransformEntityStruct.h"
#include "Starfall/CreateEntityStruct.h"
#include "Starfall/IDGenerator.h"

using std::map;
using std::vector;
using std::string; 
using std::exception;

namespace Starfall {

	class Entity : private Transform {
		friend class CreateEntityStruct;
		friend class User;
		friend class Player;

		friend class Receive;

		friend class WorldScene;


		public:
			typedef Poco::SharedPtr<Entity> Ptr;
			~Entity();

			string displayName;
			Poco::UInt32 mode; //0->Tagged, 1->Clickable, 2->Controlled By Camera
			Poco::UInt32 sessionid;

			CreateEntityStruct castCreateEntityStruct();
			TransformEntityStruct castTransformEntityStruct();

		protected:
			/* Personal mutex; for each entity's own data attributes 
			 * The player's each time a packet is read. 
			 */
			Poco::Mutex mutex; 
			void addToPath(TransformStruct transformStruct); //thread-safe 
			void clearPath();

			static Ptr Create(Poco::UInt32 sessionid=IDGenerator::Next());
			/* 
			 * Note: Lookups using a hash map are fast! The find function should run in constant time, whereas an iteration over the map would be linear.
			 */
			static vector<Poco::UInt32> Keys();
			static CreateEntityStruct FindCreateEntityStruct(Poco::UInt32 sessionid); //safe; updates reference count for Entity* in map
			static TransformEntityStruct FindTransformEntityStruct(Poco::UInt32 sessionid); //safe; updates reference count for Entity* in map
			static void Remove(Poco::UInt32 sessionid);

		private:

			v8::Isolate* isolate; //a pointer to the instance of the v8 engine this entity is using.
			v8::Persistent<v8::Context> persistentContext; //A persistent javascript context holding dynamic data such as the entity's inventory
			vector<TransformStruct> path;

			Entity(Poco::UInt32 sessionid);
			
			void Load(); //Load the entity's scripts.

			static Poco::Mutex CollectionMutex; //mutex for the map storing entities

			/* Note: 
					1) Use a hash table; search trees are used in std::map which we do not need.
					Inserting elements is cheap with a hashmap, and will not need to search much. Iteration, which we will do often, is linear.
			  */
			static Poco::HashMap<Poco::UInt32, Ptr> Map; 

	};


}
