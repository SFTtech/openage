// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include "../util/fixed_point.h"
#include "../util/misc.h"

#include "coord_nese.gen.h"
#include "coord_neseup.gen.h"

namespace openage {
namespace coord {

constexpr unsigned int phys_t_radix_pos = 16;
constexpr unsigned int phys_per_tile = (1 << phys_t_radix_pos);

using phys_t = util::FixedPoint<int64_t, 16>;


struct phys2_delta;
struct phys2;
struct phys3_delta;
struct phys3;


struct phys2_delta : CoordNeSeRelative<phys_t, phys2, phys2_delta> {
	using CoordNeSeRelative<phys_t, phys2, phys2_delta>::CoordNeSeRelative;

	phys3_delta to_phys3(phys_t up=0) const;
};

struct phys2 : CoordNeSeAbsolute<phys_t, phys2, phys2_delta> {
	using CoordNeSeAbsolute<phys_t, phys2, phys2_delta>::CoordNeSeAbsolute;
};


struct phys3_delta : CoordNeSeUpRelative<phys_t, phys3, phys3_delta> {
	using CoordNeSeUpRelative<phys_t, phys3, phys3_delta>::CoordNeSeUpRelative;
};

struct phys3 : CoordNeSeUpAbsolute<phys_t, phys3, phys3_delta> {
	using CoordNeSeUpAbsolute<phys_t, phys3, phys3_delta>::CoordNeSeUpAbsolute;
};


phys3_delta phys2_delta::to_phys3(phys_t up) const {
	return phys3_delta(this->ne, this->se, up);
}


} // namespace coord
} // namespace openage

namespace std {

template<>
struct hash<openage::coord::phys3> {
	size_t operator ()(const openage::coord::phys3 &pos) const {
		return (
			openage::util::rol<size_t, 2>(hash<openage::coord::phys_t>{}(pos.ne)) ^
			openage::util::rol<size_t, 1>(hash<openage::coord::phys_t>{}(pos.se)) ^
			                              hash<openage::coord::phys_t>{}(pos.up)   
		);
	}
};

} // namespace std
