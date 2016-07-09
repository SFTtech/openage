// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#include "phys3_serialization.h"

namespace openage {
namespace coord {

std::ostream& operator<<(std::ostream &o, const phys3 &v) {
	o << '(' << v.ne << ',' << v.se << ',' << v.up << ')';
	return o;
}

std::ostream& operator<<(std::ostream &o, const phys3_delta &v) {
	o << '(' << v.ne << ',' << v.se << ',' << v.up << ')';
	return o;
}

std::istream& operator>>(std::istream &i, phys3 &v) {
	char c;
	i >> c >> v.ne >> c >> v.se >> c >> v.up >> c;
	return i;
}

std::istream& operator>>(std::istream &i, phys3_delta &v) {
	char c;
	i >> c >> v.ne >> c >> v.se >> c >> v.up >> c;
	return i;
}

}} // openage::coord
