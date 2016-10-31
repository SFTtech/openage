// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <iostream>

namespace openage {

struct tile_range;

std::ostream& operator<<(std::ostream &o, const tile_range &v);
std::istream& operator>>(std::istream &i, tile_range &v);

} // openage
