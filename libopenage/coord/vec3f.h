// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_COORD_VEC3F_H_
#define OPENAGE_COORD_VEC3F_H_

#include "decl.h"
#include "xyz_coord.h"

namespace openage {
namespace coord {

struct vec3f : public absolute_xyz_coord<vec3f, vec3f_delta, float, 3> {
	vec3f() = default;
	vec3f(float x, float y, float z);
};

struct vec3f_delta : public relative_xyz_coord<vec3f, vec3f_delta, float, 3> {
	vec3f_delta() = default;
	vec3f_delta(float x, float y, float z);
};

} // namespace coord
} // namespace openage

#endif
