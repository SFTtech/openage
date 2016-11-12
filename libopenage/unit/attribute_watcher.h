// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <iostream>

namespace openage {

inline std::string parse_attr_name(std::istream &stream) {
	std::string name;
	stream >> name;
	return name;
}

} // namespace openage
