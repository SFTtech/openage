// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_COORD_VEC2_H_
#define OPENAGE_COORD_VEC2_H_

#include "decl.h"

#include "xyz_coord.h"

namespace openage {
namespace coord {

struct vec2 : public absolute_xyz_coord<vec2, vec2_delta, int64_t, 2 > {
	vec2() = default;
	vec2(int64_t x, int64_t y);
};

struct vec2_delta : public relative_xyz_coord<vec2, vec2_delta, int64_t, 2 >{
	vec2_delta() = default;
	vec2_delta(int64_t x, int64_t y);
};

} // namespace coord
} // namespace openage

#endif
