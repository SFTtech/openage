// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include "coord_xy.gen.h"

namespace openage {
namespace coord {

using term_t = int;


struct term_delta;
struct term;


struct term_delta : CoordXYRelative<term_t, term, term_delta> {
	using CoordXYRelative<term_t, term, term_delta>::CoordXYRelative;
};

struct term : CoordXYAbsolute<term_t, term, term_delta> {
	using CoordXYAbsolute<term_t, term, term_delta>::CoordXYAbsolute;
};


} // namespace coord
} // namespace openage
