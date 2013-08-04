//Copyright (c) 2013 Mark Farrell



#pragma once

#include "Starfall/Head.h"
#include "Starfall/Entity.h"
#include "Starfall/Transform.h"
#include "Starfall/LoginStruct.h"
#include "Starfall/Packet.h"

#include "Poco/NumberParser.h"
#include "Poco/NumberFormatter.h"
#include "Poco/Net/Net.h"
#include <exception>
#include <vector>
#include <string>
#include <iostream>

using std::cout;
using std::endl;

using std::vector;
using std::exception;
using std::string;
using std::stringstream;

namespace Starfall {

	class BufferException : public exception {
		private:
			string msg;
		public:
			BufferException(const string m="Buffer Exception: No implementation for this type.") : msg(m) { }
			const char* what() { return msg.c_str(); }
	};


	class Buffer : public std::vector<char> {
		public:
		
			string hex(); //represent the contents as a hex string
			void reset(); //reset the position of the iterator

			void writeUInt32(Poco::UInt32 value);
			void writeString(const string& value);
			void writeFloat(double value);

			Poco::UInt32 readUInt32();
			string		 readString();
			double		 readFloat();

			template<typename T> 
			void operator>> (T& into);

			template<typename T>
			void operator<< (T& out);

			/** 
			 * Description: Create an empty buffer. Initializes the iterator position.
			 */
			Buffer(); 
			/** 
			 * Description: Copy data into the buffer. Also initializes the iterator position.
			 */
			Buffer(int length, char* firstChar, char* lastChar);

		private:

			Buffer::iterator _it;
	};

	template<typename T>
	inline void Buffer::operator>> (T& into) {
		throw new BufferException();
	}

	template <>
	inline void Buffer::operator>> <Packet<Head>>(Packet<Head>& head) {
			head.setHex(this->hex());
			head->begin  = this->readUInt32(); 
			head->opcode = this->readUInt32();
			head->bodysize = this->readUInt32();
			head->end = this->readUInt32();
	}

	template <>
	inline void Buffer::operator>> <Packet<LoginStruct>>(Packet<LoginStruct>& loginPacket) {
			loginPacket.setHex(this->hex());
			loginPacket->state = this->readUInt32();
			loginPacket->userid  = this->readUInt32(); 
			loginPacket->usertype = this->readUInt32();
			loginPacket->username = this->readString();
			loginPacket->password = this->readString();
	}

	template <>
	inline void Buffer::operator>> <Packet<vector<TransformStruct>>>(Packet<vector<TransformStruct>>& transformsPacket) {
			transformsPacket.setHex(this->hex());
			Poco::UInt32 size = this->readUInt32();
			for(Poco::UInt32 i = 0; i < size; i++) {
				TransformStruct transformStruct;
				Poco::UInt32 sessionid = this->readUInt32();
				transformStruct.action = this->readUInt32();
				transformStruct.position.x = this->readFloat();
				transformStruct.position.y = this->readFloat();
				transformStruct.position.z = this->readFloat();
				transformStruct.rotation.x = this->readFloat();
				transformStruct.rotation.y = this->readFloat();
				transformStruct.rotation.z = this->readFloat();
				transformStruct.rotation.w = this->readFloat();
				transformsPacket->push_back(transformStruct);
			}
	}

	template <>
	inline void Buffer::operator>> <Packet<vector<Poco::UInt32>>>(Packet<vector<Poco::UInt32>>& collectionPacket) {
		Poco::UInt32 size = this->readUInt32();
		for(Poco::UInt32 i = 0; i < size; i++) {
			collectionPacket->push_back(this->readUInt32());
		}
	}

	template<typename T>
	inline void Buffer::operator<< (T& out) {
		throw new BufferException();
	}

	template<>
	inline void Buffer::operator<< <Transform>(Transform& transform) {
		this->writeFloat(transform.position.x);
		this->writeFloat(transform.position.y);
		this->writeFloat(transform.position.z);
		this->writeFloat(transform.rotation.x);
		this->writeFloat(transform.rotation.y);
		this->writeFloat(transform.rotation.z);
		this->writeFloat(transform.rotation.w);
	}

	template<>
	inline void Buffer::operator<< <vector<DestroyEntityStruct>>(vector<DestroyEntityStruct>& destroyEntityQueue) {
		this->writeUInt32(destroyEntityQueue.size());
		for(vector<DestroyEntityStruct>::iterator it = destroyEntityQueue.begin(); it != destroyEntityQueue.end(); it++) {
			this->writeUInt32((*it).sessionid);
		}
	}

	template<>
	inline void Buffer::operator<< <vector<CreateEntityStruct>>(vector<CreateEntityStruct>& createEntityQueue) {
		this->writeUInt32(createEntityQueue.size());
		for(vector<CreateEntityStruct>::iterator it = createEntityQueue.begin(); it != createEntityQueue.end(); it++) {
			this->writeUInt32((*it).sessionid);
			this->writeUInt32(((*it).mode));
			this->writeString((*it).displayName);
			Transform transform;
			transform.position = (*it).position;
			transform.rotation = (*it).rotation;
			(*this) << transform;
		}
	}


	template<>
	inline void Buffer::operator<< <vector<TransformEntityStruct>>(vector<TransformEntityStruct>& transformEntityQueue) {
		this->writeUInt32(transformEntityQueue.size());
		for(vector<TransformEntityStruct>::iterator it = transformEntityQueue.begin(); it != transformEntityQueue.end(); it++) {
			this->writeUInt32((*it).sessionid);
			this->writeUInt32((*it).path.size());
			for(vector<TransformStruct>::iterator itPath = (*it).path.begin();  itPath != (*it).path.end(); itPath++) {
				this->writeUInt32((*itPath).action);
				Transform transform;
				transform.position = (*itPath).position;
				transform.rotation = (*itPath).rotation;
				(*this) << transform;
			}
		}
	}

}