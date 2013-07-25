#include "Transform.h"
#include <math.h>

Position::Position() {
	this->x = 0.0;
	this->y = 0.0;
	this->z = 0.0;
}

double Position::Distance(Position a, Position b) {
	return sqrt(pow(b.x-a.x,2.0)+pow(b.y-a.y,2.0)+pow(b.z-a.z, 2.0));
}

Rotation::Rotation() {
	this->x = 0.0;
	this->y = 0.0;
	this->z = 0.0;
	this->w = 0.0;
}