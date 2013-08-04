//
//
//Copyright (c) 2013 Mark Farrell
//
//





//


//









#include "Starfall/Buffer.h"
#include <iostream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <iterator>

using namespace Starfall;

using std::cout;
using std::endl;

Buffer::Buffer() {
	this->reset();
}

Buffer::Buffer(int length, char* firstChar, char* lastChar) {
	this->reserve( this->size() + length );
	std::copy(firstChar, lastChar, std::back_inserter(*this));
	this->reset();
}

string Buffer::hex() {
	string ret;
	for(int i = 0; i < this->size(); i++) {

		/* Format the hexadecimal values that are being printed.
		 * E.g. 0 should become 00 */
		stringstream ss;
		ss << std::uppercase << std::hex << ((static_cast<Poco::UInt32>(this->data()[i]))&0xFF);
		string s;
		ss >> s;
		if(s.length() == 1) {
			s.insert(s.begin(), '0'); 
		} else if(s.length() == 0) {
			cout << "Error: The length of the parsed string should not be 0." << endl;
		}

		ret = ret + s;

		if((i+1) % 16 == 0 || (i+1) == this->size()) { //create rows
			ret = ret + "\n";
		} else { //if not a new row, insert a space
			ret = ret + " ";
		}

	}
	return ret;
}

void Buffer::reset() {
	this->_it = this->begin();
}


string Buffer::readString() {

	string value;
	Poco::UInt32 length = this->readUInt32();

	if(length > 0 && (_it + length - 1) != this->end() ) {
		for(int i = 0; i < length; i++) {
			value.push_back(*(_it + i));
		}
		_it += length;
	}

	return value;
}

Poco::UInt32 Buffer::readUInt32() {
	//Note: the end iterator refers to a past-the-end element in the container (cplusplus.com)
	//Therefore, we only need to compare our iterator to end once. Even if _it alone is past the end,
	//the same comparison will still be made.
	if( ( _it + sizeof(Poco::UInt32) - 1 ) == this->end() ) {
		// 04/12/2013 TODO: Add error message / catch case
		return 0;
	}

	Poco::UInt32 value = static_cast<Poco::UInt8>(*(_it))   << 24
			           | static_cast<Poco::UInt8>(*(_it+1)) << 16
	                   | static_cast<Poco::UInt8>(*(_it+2)) << 8
	                   | static_cast<Poco::UInt8>(*(_it+3));


	_it += sizeof(Poco::UInt32); //Increase the position of the iterator

	return value;
}

double Buffer::readFloat() {
	double ret = 0.0;
	if(!Poco::NumberParser::tryParseFloat(this->readString(), ret))
	{
		throw new BufferException("Buffer::readFloat: Parsing error. ");
	}
	return ret;
}

void Buffer::writeUInt32(Poco::UInt32 value) {
	//0xAABBCCDD
	this->push_back((value>>24)&0xFF); //0xAA
	this->push_back((value>>16)&0xFF); //0xBB
	this->push_back((value>>8)&0xFF); //0xCC
	this->push_back((value)&0xFF); ///0xDD
}

void Buffer::writeString(const string& value) {
	this->writeUInt32(value.size());
	this->insert(this->end(), value.begin(), value.end());
}

void Buffer::writeFloat(double value) {
	this->writeString(Poco::NumberFormatter::format(value));
}