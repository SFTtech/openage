// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <cmath>

namespace openage {
namespace math {

template<typename T>
T square(T arg) {
	return arg * arg;
}


template<typename T>
T hypot3(T x, T y, T z) {
	return sqrt(x * x + y * y + z * z);
}

}} // openage::util
