// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_COORD_PHYS3_H_
#define OPENAGE_COORD_PHYS3_H_

#include <functional>

#include "decl.h"
#include "../util/misc.h"
#include "ne_se_up_coord.h"

namespace openage {
namespace coord {

struct phys3 : public absolute_ne_se_up_coord<phys3, phys3_delta, phys_t, 3> {
	phys3() = default;
	phys3(phys_t ne, phys_t se, phys_t up);

	phys2 to_phys2() const;
	camgame to_camgame() const;
	tile3 to_tile3() const;
	phys3_delta get_fraction();
};

struct phys3_delta : public relative_ne_se_up_coord<phys3, phys3_delta, phys_t, 3>  {
	phys3_delta() = default;
	phys3_delta(phys_t ne, phys_t se, phys_t up);

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

SCALAR_MULOPERATOR_DECL(phys_t, phys3)
SCALAR_MULOPERATOR_DECL(phys_t, phys3_delta)


} // namespace coord
} // namespace openage

namespace std {

/**
 * Hash function for phys3 coordinates.
 */
template<>
struct hash<openage::coord::phys3> {
	size_t operator ()(const openage::coord::phys3 &pos) const {
		size_t nehash = std::hash<openage::coord::phys_t>{}(pos.ne);
		size_t sehash = std::hash<openage::coord::phys_t>{}(pos.se);
		return openage::util::rol<size_t, 1>(nehash) ^ sehash;
	}
};

} // namespace std


#endif
