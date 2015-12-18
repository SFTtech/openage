// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_COORD_PHYS2_H_
#define OPENAGE_COORD_PHYS2_H_

#include "decl.h"
#include "ne_se_up_coord.h"

namespace openage {
namespace coord {

struct phys2 : public absolute_ne_se_up_coord<phys2, phys2_delta, phys_t, 2> {
	phys2() = default;
	phys2(phys_t ne, phys_t se);

	phys3 to_phys3(phys_t up = 0) const;
	tile to_tile() const;
	phys2_delta get_fraction() const;
};

struct phys2_delta : public relative_ne_se_up_coord<phys2, phys2_delta, phys_t, 2> {
	phys2_delta() = default;
	phys2_delta(phys_t ne, phys_t se);

	phys3_delta to_phys3(phys_t up = 0) const;

	//allowing conversions of phys deltas to tile deltas isn't such a great
	//idea. you never know whether a phys change of 0.9 will _actually_
	//change the tile.
};

} // namespace coord
} // namespace openage

#endif
