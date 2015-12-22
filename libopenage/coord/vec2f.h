// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_COORD_VEC2F_H_
#define OPENAGE_COORD_VEC2F_H_

#include "decl.h"
#include "xyz_coord.h"

namespace openage {
namespace coord {

struct vec2f : public absolute_xyz_coord< vec2f, vec2f_delta, float, 2  > {
	vec2f() = default;
	vec2f(float x, float y);
};

struct vec2f_delta : public relative_xyz_coord< vec2f, vec2f_delta, float, 2  >  {
	vec2f_delta() = default;
	vec2f_delta(float x, float y);
};

} // namespace coord
} // namespace openage

#endif
