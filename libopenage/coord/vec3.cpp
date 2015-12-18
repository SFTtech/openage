// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#define GEN_IMPL_VEC3_H
#include "vec3.h"

namespace openage {
namespace coord {

vec3::vec3(int64_t x, int64_t y, int64_t z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

vec3_delta::vec3_delta(int64_t x, int64_t y, int64_t z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

} // namespace coord
} // namespace openage
