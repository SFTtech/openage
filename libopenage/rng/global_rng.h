// Copyright 2015-2016 the openage authors. See copying.md for legal info.
#pragma once

/** @file
 * This file contains functions for the global random number generator.
 *
 * Please avoid using it, instead, create rng objects for your current
 * class as members.
 */


namespace openage {
namespace rng {

/**  Returns the value of the global seed. */
uint64_t global_seed();

/** Returns a random number from the global rng. */
uint64_t random();

/** Returns a random number n [lower, upper) from the global rng. */
uint64_t random_range(uint64_t lower, uint64_t upper);

/** Returns a double in [0, 1) from the global rng. */
double real();

/** Returns a double in [lower, upper) from the global rng. */
double real_range(double lower, double upper);

} // namespace rng
} // namespace openage
