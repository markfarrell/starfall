//Copyright (c) 2013 Mark Farrell
#pragma once

#include "Starfall/Head.h"
#include "Starfall/CreateEntityStruct.h"
#include "Starfall/TransformEntityStruct.h"
#include "Starfall/DestroyEntityStruct.h"
#include "Starfall/ObjectsStruct.h"
#include "Starfall/ObjectsUpdateStruct.h"
#include "Starfall/Transform.h"
#include "Starfall/LoginStruct.h"
#include "Starfall/Packet.h"
#include "Starfall/BufferBase.h"

namespace Starfall {
	template <>
	inline void Buffer::operator>> < Packet<Head> >(Packet<Head>& head) {
			head.setHex(this->hex());
			head->begin  = this->readUInt32(); 
			head->opcode = this->readUInt32();
			head->bodysize = this->readUInt32();
			head->end = this->readUInt32();
	}

	template <>
	inline void Buffer::operator>> < Packet<LoginStruct> >(Packet<LoginStruct>& loginPacket) {
			loginPacket.setHex(this->hex());
			loginPacket->state = this->readUInt32();
			loginPacket->userid  = this->readUInt32(); 
			loginPacket->usertype = this->readUInt32();
			loginPacket->username = this->readString();
			loginPacket->password = this->readString();
	}

	template <>
	inline void Buffer::operator>> < Packet< vector<TransformStruct> > >(Packet< vector<TransformStruct> >& transformsPacket) {
			transformsPacket.setHex(this->hex());
			Poco::UInt32 size = this->readUInt32();
			for(Poco::UInt32 i = 0; i < size; i++) {
				TransformStruct transformStruct;
				Poco::UInt32 sessionid = this->readUInt32();
				transformStruct.action = this->readUInt32();
				transformStruct.position.x = this->readFloat();
				transformStruct.position.y = this->readFloat();
				transformStruct.position.z = this->readFloat();
				transformStruct.orientation.x = this->readFloat();
				transformStruct.orientation.y = this->readFloat();
				transformStruct.orientation.z = this->readFloat();
				transformStruct.orientation.w = this->readFloat();
				transformsPacket->push_back(transformStruct);
			}
	}

	template <>
	inline void Buffer::operator>> < Packet< vector<Poco::UInt32> > >(Packet< vector<Poco::UInt32> >& collectionPacket) {
		Poco::UInt32 size = this->readUInt32();
		for(Poco::UInt32 i = 0; i < size; i++) {
			collectionPacket->push_back(this->readUInt32());
		}
	}

	template <>
	inline void Buffer::operator>> < vector<Poco::UInt32> >( vector<Poco::UInt32>& list) {
		Poco::UInt32 size = this->readUInt32();
		for(Poco::UInt32 i = 0; i < size; i++) {
			list.push_back(this->readUInt32());
		}
	}


	template <>
	inline void Buffer::operator>> < Packet<ObjectsStruct> >( Packet<ObjectsStruct>& objectsStruct) {
		objectsStruct->state = this->readUInt32();
		objectsStruct->farClipDistance = this->readFloat();
		(*this) >> objectsStruct->ids;
	}



	template<>
	inline void Buffer::operator<< <Head> (Head& head) {
		this->writeUInt32(head.begin);
		this->writeUInt32(head.opcode);
		this->writeUInt32(head.bodysize);
		this->writeUInt32(head.end);
	}

	template<>
	inline void Buffer::operator<< <LoginStruct> (LoginStruct& loginStruct) {
		this->writeUInt32(loginStruct.state);
		this->writeUInt32(loginStruct.userid);
		this->writeUInt32(loginStruct.usertype);
		this->writeString(loginStruct.username);
		this->writeString(loginStruct.password);
	}

	template<>
	inline void Buffer::operator<< <Transform>(Transform& transform) {
		this->writeFloat(transform.position.x);
		this->writeFloat(transform.position.y);
		this->writeFloat(transform.position.z);
		this->writeFloat(transform.orientation.x);
		this->writeFloat(transform.orientation.y);
		this->writeFloat(transform.orientation.z);
		this->writeFloat(transform.orientation.w);
	}

	template<>
	inline void Buffer::operator<< < vector<DestroyEntityStruct> >(vector<DestroyEntityStruct>& destroyEntityQueue) {
		this->writeUInt32(destroyEntityQueue.size());
		for(vector<DestroyEntityStruct>::iterator it = destroyEntityQueue.begin(); it != destroyEntityQueue.end(); it++) {
			this->writeUInt32((*it).sessionid);
		}
	}

	template<>
	inline void Buffer::operator<< < vector<CreateEntityStruct> >(vector<CreateEntityStruct>& createEntityQueue) {
		this->writeUInt32(createEntityQueue.size());
		for(vector<CreateEntityStruct>::iterator it = createEntityQueue.begin(); it != createEntityQueue.end(); it++) {
			this->writeUInt32((*it).sessionid);
			this->writeUInt32(((*it).mode));
			this->writeString((*it).displayName);
			Transform transform;
			transform.position = (*it).position;
			transform.orientation = (*it).orientation;
			(*this) << transform;
		}
	}


	template<>
	inline void Buffer::operator<< < vector<TransformEntityStruct> >(vector<TransformEntityStruct>& transformEntityQueue) {
		this->writeUInt32(transformEntityQueue.size());
		for(vector<TransformEntityStruct>::iterator it = transformEntityQueue.begin(); it != transformEntityQueue.end(); it++) {
			this->writeUInt32((*it).sessionid);
			this->writeUInt32((*it).path.size());
			for(vector<TransformStruct>::iterator itPath = (*it).path.begin();  itPath != (*it).path.end(); itPath++) {
				this->writeUInt32((*itPath).action);
				Transform transform;
				transform.position = (*itPath).position;
				transform.orientation = (*itPath).orientation;
				(*this) << transform;
			}
		}
	}

	template<>
	inline void Buffer::operator<< <ObjectsUpdateStruct>( ObjectsUpdateStruct& updateStruct) {
		this->writeUInt32(updateStruct.state); //set state; allow another objectsData packet to be sent.
		(*this) << updateStruct.createEntities;
		(*this) << updateStruct.destroyEntities;
	}
}
