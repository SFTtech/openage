// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#include "vec3f.h"

namespace openage {
namespace coord {

vec3f::vec3f(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

vec3f_delta::vec3f_delta(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

} // namespace coord
} // namespace openage
