// Copyright 2013-2014 the openage authors. See copying.md for legal info.

#define GEN_IMPL_PHYS2_CPP
#include "phys2.h"

#include "phys3.h"
#include "tile.h"

namespace openage {
namespace coord {

phys3 phys2::to_phys3(phys_t up) const {
	return phys3 {ne, se, up};
}

phys3_delta phys2_delta::to_phys3(phys_t up) const {
	return phys3_delta {ne, se, up};
}

tile phys2::to_tile() const {
	tile result;
	result.ne = (ne >> settings::phys_t_radix_pos);
	result.se = (se >> settings::phys_t_radix_pos);
	return result;
}

phys2_delta phys2::get_fraction() const {
	phys2_delta result;

	// define a bitmask that keeps the last n bits
	decltype(result.ne) bitmask = ((1 << settings::phys_t_radix_pos) - 1);

	result.ne = (ne & bitmask);
	result.se = (se & bitmask);
	return result;
}

} // namespace coord
} // namespace openage
