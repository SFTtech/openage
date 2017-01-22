// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#include <iostream>

#include "perlin.h"
#include "../testing/testing.h"

namespace openage {
namespace perlin {
namespace tests {

void perlin() {
	{
		auto perlin_a = perlin::Perlin<2>{56983523, 10};
		auto perlin_b = perlin::Perlin<2>{56983523, 10};
		auto vector = util::Vector<2, perlin::coord_t>{1, 2};
		TESTEQUALS(
			perlin_a.noise_value(vector),
			perlin_b.noise_value(vector)
		);
	}
	{
		auto perlin_a = perlin::Perlin<8>{75983224, 50};
		auto perlin_b = perlin::Perlin<8>{75983224, 50};
		auto vector = util::Vector<8, perlin::coord_t>{
			10932,
			632095,
			7634,
			7438976,
			145,
			320,
			743,
			4657
		};
		TESTEQUALS(
			perlin_a.noise_value(vector),
			perlin_b.noise_value(vector)
		);
	}
}

}}} // openage::perlin::tests
