// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_COORD_PHYS3_H_
#define OPENAGE_COORD_PHYS3_H_

#include "decl.h"

#define MEMBERS ne, se, up
#define SCALAR_TYPE phys_t
#define ABSOLUTE_TYPE phys3
#define RELATIVE_TYPE phys3_delta

namespace openage {
namespace coord {

struct phys3 {
	phys_t ne, se, up;

	#include "ops/abs.h"

	phys2 to_phys2() const;
	camgame to_camgame() const;
	tile3 to_tile3() const;
	phys3_delta get_fraction();
};

struct phys3_delta {
	phys_t ne, se, up;

	#include "ops/rel.h"

	phys2_delta to_phys2() const;
	camgame_delta to_camgame() const;
};

/**
 * distance between two points
 */
coord::phys_t distance(const coord::phys3 &a, const coord::phys3 &b);

/**
 * modify the length of a phys3_delta vector
 */
coord::phys3_delta normalize(const coord::phys3_delta &a, const coord::phys_t &length);

#include "ops/free.h"

#ifdef GEN_IMPL_PHYS3_CPP
#include "ops/impl.h"
#endif //GEN_IMPL_PHYS3_CPP

} //namespace coord
} //namespace openage

#undef MEMBERS
#undef RELATIVE_TYPE
#undef ABSOLUTE_TYPE
#undef SCALAR_TYPE

#endif
