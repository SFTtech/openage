// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#include "box_muller.h"

namespace openage {
namespace rng {

void BoxMuller::generate() {
	// FIXME: u_1 might be 0
	double u_1 = this->random();
	double u_2 = this->random();
	double part_a = sqrt((-2) * log(u_1));
	double part_b = 2 * math::PI * u_2;
	this->yield(part_a * cos(part_b));
	this->yield(part_a * sin(part_b));
}

BoxMuller::BoxMuller(std::function<double()> random)
	:
	random{random} {};

}} // openage::rng
