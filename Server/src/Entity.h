#pragma once
#include "Poco/Net/Net.h"
#include "Poco/SharedPtr.h"
#include "Poco/Mutex.h"
#include "Poco/HashMap.h"
#include "Transform.h"
#include <exception>
#include <vector>
#include <string>

using std::vector;
using std::string; 
using std::exception;

class IDException : public exception { 
	private:
		string msg;
	public:
		IDException(const string m="IDException: The maximum possible ID has been exceeded.") : msg(m) { }
		const char* what() { return msg.c_str(); }
};

class IDGenerator {
	friend class Entity;
	private:
		static Poco::Mutex mutex; 
		static Poco::UInt32 id;
	protected:
		static Poco::UInt32 next(); //uses a mutex to stop simultaneous access
};

class DestroyEntityStruct {
	public:
		Poco::UInt32 sessionid;
		DestroyEntityStruct();
};

class CreateEntityStruct : public Transform {
	public:
		Poco::UInt32 sessionid;
		Poco::UInt32 mode;
		string displayName;
		string appearance;
		CreateEntityStruct();
};

class TransformStruct : public Transform {
	public:
		Poco::UInt32 action;
		TransformStruct();
};

class TransformEntityStruct {
	public:
		vector<TransformStruct> path;
		Poco::UInt32 sessionid;
		TransformEntityStruct();
};

class Entity : private Transform {
	friend class CreateEntityStruct;
	friend class User;
	friend class Player;
	friend class Receive;
	public:
		typedef Poco::SharedPtr<Entity> Ptr;

		string displayName;
		string appearance;
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

		static Ptr Create();
		/* 
		 * Note: Lookups using a hash map are fast! The find function should run in constant time, whereas an iteration over the map would be linear.
		 */
		static vector<Poco::UInt32> Keys();
		static CreateEntityStruct FindCreateEntityStruct(Poco::UInt32 sessionid); //safe; updates reference count for Entity* in map
		static TransformEntityStruct FindTransformEntityStruct(Poco::UInt32 sessionid); //safe; updates reference count for Entity* in map
		static void Remove(Poco::UInt32 sessionid);

	private:

		vector<TransformStruct> path;
		Entity();

		static Poco::Mutex CollectionMutex; //mutex for the map storing entities

	
		/* Note: 
				1) Use a hash table; search trees are used in std::map which we do not need.
				Inserting elements is cheap with a hashmap, and will not need to search much. Iteration, which we will do often, is linear.
		  */
		static Poco::HashMap<Poco::UInt32, Ptr> Map; 


};