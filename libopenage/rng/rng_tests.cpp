// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "rng.h"

#include "../log/log.h"
#include "../error/error.h"
#include "../testing/testing.h"

#include <limits>
#include <vector>
#include <cmath>

namespace openage {
namespace rng {
namespace tests {

/**
 * This is the number of generation repetitions
 */
constexpr size_t num_rand = (1 << 18);

/**
 * Allow the result to be in this range of the expected
 * perfect distribution.
 */
constexpr float difference_factor = 0.1;


/**
 * Tests the distribution of the bit generator.
 *
 * We generate many many random numbers, and select a bucket by it.
 * After n random numbers, each bucket should have been selected
 * n/bucket_count times.
 */
void freq_dist() {
	// one random sample from the generator.
	uint64_t sample;

	// number of possible chars, likely 256.
	// this is bucket_count.
	constexpr size_t dsize = std::numeric_limits<unsigned char>::max() + 1;

	// each bucket should be selected n/bucket_count times.
	// each sample contains multiple chars.
	constexpr float mean = (sizeof(sample) * num_rand) / dsize;

	// maximum difference from the expected mean.
	constexpr float max_diff = mean * difference_factor;

	// holds one entry for each char bit.
	size_t buckets[dsize];

	for (auto &dat : buckets) {
		dat = 0;
	}

	RNG myrng{time_seed()};

	for (size_t i = 0; i < num_rand; i++) {
		sample = myrng();  // get a random value
		unsigned char *valptr = reinterpret_cast<unsigned char*>(&sample);

		// for each bit of the generated value:
		// increment the "weight" of the bit.
		for (size_t j = 0; j < sizeof(sample); j++) {
			// randomly increment one of the buckets
			unsigned char rbyte = valptr[j];
			buckets[rbyte] += 1;
		}
	}

	// test if each bucket was selected the amount we expected.
	for (ssize_t count : buckets) {
		std::abs(mean - count) > max_diff and TESTFAIL;
	}
}

/**
 * Tests the distribution of the boolean generation.
 *
 * For n draws it should result in n/2 trues and n/2 falses.
 */
void bool_dist() {
	constexpr float expected = num_rand / 2;
	constexpr float max_diff = expected * difference_factor;

	RNG mybool{random_seed()};
	size_t num_true = 0;

	for (size_t i = 0; i < num_rand; i++) {
		if (mybool() & 1) {
			num_true += 1;
		}
	}

	std::abs(num_true - expected) > max_diff and TESTFAIL;
}

/**
 * Tests the distribution of the floating point numbers.
 *
 * Perform the test by placing the [0, 1] range into buckets.
 * each bucket should be drawn n/bucket_count times.
 *
 * Also, the sum of all numbers drawn should be n/2.
 */
void real_dist() {
	constexpr size_t bckt_cnt    = num_rand >> 11;
	constexpr float  mean        = num_rand / bckt_cnt;
	constexpr float  max_diff    = mean * difference_factor;
	constexpr float  max_sumdiff = (num_rand/2) * difference_factor;
	ssize_t          buckets[bckt_cnt+1];

	for (auto &dat : buckets) {
		dat = 0;
	}

	double sum = 0;

	RNG myrng{time_seed()};

	for (size_t i = 0; i < num_rand; i++) {
		double val = myrng.real();
		sum += val;
		buckets[static_cast<size_t>(val * bckt_cnt)] += 1;
	}

	std::abs(sum - (num_rand / 2)) > max_sumdiff and TESTFAIL;

	for (size_t i = 0; i < bckt_cnt; i++) {
		std::abs(mean - buckets[i]) > max_diff and TESTFAIL;
	}

	buckets[bckt_cnt] and TESTFAIL;
}



/**
 * Tests reproducibility.
 */
void reproduce() {
	auto val = time_seed();
	RNG test0{val};
	RNG test1{val};

	for (size_t i = 0; i < num_rand; i++) {
		test0() == test1() or TESTFAIL;
	}

	// seed both rngs
	char initstr[] = "abcdefghijk";
	test0.seed(initstr, sizeof(initstr));
	test1.seed(initstr, sizeof(initstr));
	for (size_t i = 0; i < num_rand; i++) {
		test0() == test1() or TESTFAIL;
	}

	// same result expected, as we only use the first 3 chars as seed.
	char initstr2[] = "abcqq";
	test0.seed(initstr, 3);
	test1.seed(initstr2, 3);
	for (size_t i = 0; i < num_rand; i++) {
		test0() == test1() or TESTFAIL;
	}

	// make sure that seeding with nullptr fails.
	TESTTHROWS(test0.seed(nullptr, 0));
}

/**
 * Tests the serializing functionality.
 */
void serialize() {
	// create seeded rng
	// and duplicate state to another one.
	RNG test0{random_seed()};
	RNG test1{test0.to_string()};
	for (size_t i = 0; i < num_rand; i++) {
		test0() == test1() or TESTFAIL;
	}

	TESTTHROWS(test0.from_string("100 aa"));
	TESTTHROWS(test0.from_string(""));
	TESTTHROWS(test0.from_string("100"));
	TESTTHROWS(test0.from_string("100 "));
}


/**
 * Tests filling integers.
 */
void fill() {
	constexpr size_t n = 1 << 7;
	uint64_t data[n];

	RNG test0{random_seed()};
	RNG test1{test0};

	test0.fill(data, n);

	for (size_t i = 0; i < n; i++) {
		test1() == data[i] or TESTFAIL;
	}

	for (size_t i = 0; i < num_rand; i++) {
		test0() == test1() or TESTFAIL;
	}
}


/**
 * Tests filling doubles.
 */
void fill_real() {
	constexpr size_t n = 1 << 7;
	RNG test0{time_seed()};
	RNG test1{test0};
	double data[n];

	test1.fill_real(data, n);
	for (size_t i = 0; i < n; i++) {
		test0.real() == data[i] or TESTFAIL;
	}

	for (size_t i = 0; i < num_rand; i++) {
		test0() == test1() or TESTFAIL;
	}
}


void run() {
	freq_dist();
	bool_dist();
	real_dist();
	reproduce();
	fill();
	fill_real();
}


}}} // namespace openage::rng::tests
