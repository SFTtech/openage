// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <iostream>

#include "phys3.h"

namespace openage {
namespace coord {

std::ostream& operator<<(std::ostream &o, const phys3 &v);
std::ostream& operator<<(std::ostream &o, const phys3_delta &v);

std::istream& operator>>(std::istream &i, phys3 &v);
std::istream& operator>>(std::istream &i, phys3_delta &v);

}} // openage::coord
