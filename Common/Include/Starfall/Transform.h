//Copyright (c) 2013 Mark Farrell
#pragma once

#include <Poco/Foundation.h>

namespace Starfall {

	class Position { 
		public:
			double x;
			double y;
			double z;
			Position();
			static double Distance(Position a, Position b); //copy for thread safety
	};

	class Rotation { 
		public:
			double x;
			double y;
			double z;
			double w;
			Rotation();
	};

	class Transform {
		public:
			Position position;
			Rotation rotation;
	};

}