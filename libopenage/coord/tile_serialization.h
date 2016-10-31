// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <iostream>

#include "tile.h"

namespace openage {
namespace coord {

std::ostream& operator<<(std::ostream &o, const tile &v);
std::ostream& operator<<(std::ostream &o, const tile_delta &v);

std::istream& operator>>(std::istream &i, tile &v);
std::istream& operator>>(std::istream &i, tile_delta &v);

}} // openage::coord
