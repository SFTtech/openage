// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#define GEN_IMPL_VEC2_CPP
#include "vec2.h"

namespace openage {
namespace coord {

vec2::vec2(int64_t x, int64_t y)
{
	this->x = x;
	this->y = y;
}

vec2_delta::vec2_delta(int64_t x, int64_t y)
{
	this->x = x;
	this->y = y;
}

} // namespace coord
} // namespace openage
