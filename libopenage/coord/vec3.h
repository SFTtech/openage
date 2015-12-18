// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_COORD_VEC3_H_
#define OPENAGE_COORD_VEC3_H_

#include "decl.h"
#include "xyz_coord.h"

namespace openage {
namespace coord {

struct vec3 : public absolute_xyz_coord< vec3, vec3_delta, int64_t, 3 > {
	vec3() = default;
	vec3(int64_t x, int64_t y, int64_t z);
};

struct vec3_delta : public relative_xyz_coord< vec3, vec3_delta, int64_t, 3 >  {
	vec3_delta() = default;
	vec3_delta(int64_t x, int64_t y, int64_t z);
};


} // namespace coord
} // namespace openage

#endif
