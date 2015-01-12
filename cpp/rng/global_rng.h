// Copyright 2015-2015 the openage authors. See copying.md for legal info.
#ifndef OPENAGE_RNG_GLOBAL_RNG_H_
#define OPENAGE_RNG_GLOBAL_RNG_H_

#include <cstdint>

namespace openage {
namespace rng {

//!Returns the value of the global seed
uint64_t global_seed();

//!Returns a random number from the global rng
uint64_t random();

//!Returns a random number n [lower, upper) from the global rng
uint64_t random_range(uint64_t lower, uint64_t upper);

//!Returns a double in [0, 1) from the global rng
double real();

//!Returns a double in [lower, upper) from the global rng
double real_range(double lower, double upper);

} //namespace rng
} //namespace openage
#endif
