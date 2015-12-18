// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_COORD_TILE3_H_
#define OPENAGE_COORD_TILE3_H_

#include "decl.h"
#include "ne_se_up_coord.h"
#include "phys3.h"

namespace openage {
namespace coord {

struct tile3 : public absolute_ne_se_up_coord<tile3, tile3_delta, tile_t, 3> {
	tile3() = default;
	tile3(phys_t ne, phys_t se, phys_t up);

	tile to_tile() const;
	phys3 to_phys3(phys3_delta frac = {settings::phys_per_tile / 2, settings::phys_per_tile / 2, 0});
};

struct tile3_delta : public relative_ne_se_up_coord<tile3, tile3_delta, tile_t, 3> {
	tile3_delta() = default;
	tile3_delta(phys_t ne, phys_t se, phys_t up);

	tile_delta to_tile() const;
};

} // namespace coord
} // namespace openage

#endif
