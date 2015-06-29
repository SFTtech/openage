// Copyright 2013-2015 the openage authors. See copying.md for legal info.

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
	return tile{ ne.to_int(), se.to_int() };
}

phys2_delta phys2::get_fraction() const {
	phys2_delta result;

	result.ne = ne.get_fraction();
	result.se = se.get_fraction();
	return result;
}

} // namespace coord
} // namespace openage
