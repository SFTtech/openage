// Copyright 2015-2016 the openage authors. See copying.md for legal info.
#include "rng.h"
#include "global_rng.h"

#include <mutex>

namespace openage {
namespace rng {

static std::mutex rand_mutex;
static RNG global_rand_gen(global_seed());

uint64_t global_seed() {
	static const uint64_t seed = random_seed();
	return seed;
}

uint64_t random() {
	std::lock_guard<std::mutex> rand_lock(rand_mutex);
	return global_rand_gen.random();
}

uint64_t random_range(uint64_t lower, uint64_t upper) {
	std::lock_guard<std::mutex> rand_lock(rand_mutex);
	return global_rand_gen.random_range(lower, upper);
}

double real() {
	std::lock_guard<std::mutex> rand_lock(rand_mutex);
	return global_rand_gen.real();
}

double real_range(double lower, double upper) {
	std::lock_guard<std::mutex> rand_lock(rand_mutex);
	return global_rand_gen.real_range(lower, upper);
}

}} // rng::openage
