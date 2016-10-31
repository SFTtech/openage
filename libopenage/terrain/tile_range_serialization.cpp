// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#include "tile_range_serialization.h"

#include "terrain_object.h"
#include "../coord/tile_serialization.h"
#include "../coord/phys3_serialization.h"

namespace openage {

std::ostream& operator<<(std::ostream &o, const tile_range &v) {
	o << '(' << v.start << ',' << v.end << ',' << v.draw << ')';
	return o;
}

std::istream& operator>>(std::istream &i, tile_range &v) {
	char c;
	i >> c >> v.start >> c >> v.end >> c >> v.draw >> c;
	return i;
}

} // openage
