// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#include <iostream>

#include "perlin.h"
#include "../testing/testing.h"

namespace openage {
namespace perlin {
namespace tests {

void perlin() {
	auto perlin = perlin::Perlin<2>{56983523, 10};
	auto vec = util::Vector<2, perlin::value_t>{1, 2};
	std::cout << perlin.noise_value(vec) << std::endl;
	TESTFAILMSG("Not fully implemented yet.");
}

}}} // openage::perlin::tests
