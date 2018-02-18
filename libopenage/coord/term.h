// Copyright 2016-2018 the openage authors. See copying.md for legal info.

#pragma once

#include "coord_xy.gen.h"
#include "declarations.h"

namespace openage {
namespace coord {


/*
 * Terminal-related coordinate systems.
 * See doc/code/coordinate-systems.md for more information.
 */


struct term_delta : CoordXYRelative<term_t, term, term_delta> {
	using CoordXYRelative<term_t, term, term_delta>::CoordXYRelative;
};

struct term : CoordXYAbsolute<term_t, term, term_delta> {
	using CoordXYAbsolute<term_t, term, term_delta>::CoordXYAbsolute;
};


} // namespace coord
} // namespace openage
