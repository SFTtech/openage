// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#include "tile_serialization.h"

namespace openage {
namespace coord {

std::ostream& operator<<(std::ostream &o, const tile &v) {
	o << '(' << v.ne << ',' << v.se << ')';
	return o;
}

std::ostream& operator<<(std::ostream &o, const tile_delta &v) {
	o << '(' << v.ne << ',' << v.se << ')';
	return o;
}

std::istream& operator>>(std::istream &i, tile &v) {
	char c;
	i >> c >> v.ne >> c >> v.se >> c;
	return i;
}

std::istream& operator>>(std::istream &i, tile_delta &v) {
	char c;
	i >> c >> v.ne >> c >> v.se >> c;
	return i;
}

}} // openage::coord
