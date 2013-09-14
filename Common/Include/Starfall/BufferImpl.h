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
	inline void Buffer::operator>> <Transform> (Transform& transform) {
			transform.position.x = this->readFloat();
			transform.position.y = this->readFloat();
			transform.position.z = this->readFloat();
			transform.orientation.x = this->readFloat();
			transform.orientation.y = this->readFloat();
			transform.orientation.z = this->readFloat();
			transform.orientation.w = this->readFloat();
	}


	template <>
	inline void Buffer::operator>> < vector<CreateEntityStruct> >( vector<CreateEntityStruct>& createEntities) {

		Poco::UInt32 size = this->readUInt32();
		for(Poco::UInt32 i = 0; i < size; i++) {

			CreateEntityStruct createEntityStruct;
			Transform transform;

			createEntityStruct.sessionid = this->readUInt32();
			createEntityStruct.mode = this->readUInt32();
			createEntityStruct.displayName = this->readUInt32();

			(*this) >> transform;

			createEntityStruct.position = transform.position;
			createEntityStruct.orientation = transform.orientation;

			createEntities.push_back(createEntityStruct);

		}
	}

	template <>
	inline void Buffer::operator>> < vector<DestroyEntityStruct> >( vector<DestroyEntityStruct>& destroyEntities) {

		Poco::UInt32 size = this->readUInt32();

		for(Poco::UInt32 i = 0; i < size; i++) {

			DestroyEntityStruct destroyEntityStruct;
			
			destroyEntityStruct.sessionid = this->readUInt32();

			destroyEntities.push_back(destroyEntityStruct);

		}
	}

	template <>
	inline void Buffer::operator>> < vector<TransformStruct> >(vector<TransformStruct>& transformStructs) {

		Poco::UInt32 size = this->readUInt32();
		for(Poco::UInt32 i = 0; i < size; i++) {

			TransformStruct transformStruct;
			Transform transform;

			transformStruct.action = this->readUInt32();

			(*this) >> transform;

			transformStruct.position = transform.position;
			transformStruct.orientation = transform.orientation;

			transformStructs.push_back(transformStruct);
		}
	}

	template <>
	inline void Buffer::operator>> < Packet< vector<TransformStruct> > >(Packet< vector<TransformStruct> >& transformsPacket) {
		transformsPacket.setHex(this->hex());
		(*this) >> transformsPacket.value();
	}

	template <>
	inline void Buffer::operator>> < Packet< vector<TransformEntityStruct> > >(Packet< vector<TransformEntityStruct> >& transformEntityStructs) {
		transformEntityStructs.setHex(this->hex());

		Poco::UInt32 size = this->readUInt32();
		for(Poco::UInt32 i = 0; i < size; i++) {

			TransformEntityStruct transformEntityStruct;

			transformEntityStruct.sessionid = this->readUInt32();

			(*this) >> transformEntityStruct.path;

			transformEntityStructs->push_back(transformEntityStruct);
			
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

	template <>
	inline void Buffer::operator>> < Packet<ObjectsUpdateStruct> >( Packet<ObjectsUpdateStruct>& updateStruct) {
		updateStruct->state = this->readUInt32();
		(*this) >> updateStruct->createEntities;
		(*this) >> updateStruct->destroyEntities;
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
	inline void Buffer::operator<< < vector<TransformStruct> >(vector<TransformStruct>& path){
		this->writeUInt32(path.size());
		for(vector<TransformStruct>::iterator itPath = path.begin();  itPath != path.end(); itPath++) {
			this->writeUInt32((*itPath).action);
			Transform transform;
			transform.position = (*itPath).position;
			transform.orientation = (*itPath).orientation;
			(*this) << transform;
		}
	}

	template<>
	inline void Buffer::operator<< < vector<TransformEntityStruct> >(vector<TransformEntityStruct>& transformEntityQueue) {
		this->writeUInt32(transformEntityQueue.size());
		for(vector<TransformEntityStruct>::iterator it = transformEntityQueue.begin(); it != transformEntityQueue.end(); it++) {
			this->writeUInt32((*it).sessionid);
			(*this) << (*it).path;
		}
	}

	template<>
	inline void Buffer::operator<< <ObjectsUpdateStruct>( ObjectsUpdateStruct& updateStruct) {
		this->writeUInt32(updateStruct.state); //set state; allow another objectsData packet to be sent.
		(*this) << updateStruct.createEntities;
		(*this) << updateStruct.destroyEntities;
	}
}
