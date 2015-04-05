// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "rng.h"

#include "../log/log.h"
#include "../util/error.h"

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
 * Test the distribution of the bit generator
 *
 * We generate many many random numbers, and select a bucket by it.
 * After n random numbers, each bucket should have been selected
 * n/bucket_count times.
 */
static int rng_freq_dist() {
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
		if (std::abs(mean - count) > max_diff) {
			return 0;
		}
	}
	return -1;
}

/**
 * Test the distribution of the boolean generation.
 *
 * For n draws it should result in n/2 trues and n/2 falses.
 */
static int rng_bool_dist() {
	constexpr float expected = num_rand / 2;
	constexpr float max_diff = expected * difference_factor;

	RNG mybool{random_seed()};
	size_t num_true = 0;

	for (size_t i = 0; i < num_rand; i++) {
		if (mybool() & 1) {
			num_true += 1;
		}
	}

	if (std::abs(num_true - expected) > max_diff) {
		return 0;
	}

	return -1;
}

/**
 * Test the distribution of the floating point numbers.
 *
 * Perform the test by placing the [0, 1] range into buckets.
 * each bucket should be drawn n/bucket_count times.
 *
 * Also, the sum of all numbers drawn should be n/2.
 */
static int rng_real_dist() {
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

	if (std::abs(sum - (num_rand / 2)) > max_sumdiff) {
		return 0;
	}

	for (size_t i = 0; i < bckt_cnt; i++) {
		if (std::abs(mean - buckets[i]) > max_diff) {
			return 1;
		}
	}

	if (buckets[bckt_cnt]) {
		return 2;
	}

	return -1;
}


/**
 * returns last arg if calling rn1() != rn2()
 * this is repeated num_rand times.
 */
#define ret_if_neq(rn1, rn2, val) \
	for (size_t qq = 0; qq < num_rand; qq++) { \
		if (rn1() != rn2()) { \
			return val; \
		} \
	}

/**
 * test if two rngs produce the same values with the same seeds.
 */
static int rng_reproduce() {
	auto val = time_seed();
	RNG test{val};
	RNG test2{val};
	ret_if_neq(test, test2, 0);

	// seed both rngs
	char initstr[] = "abcdefghijk";
	test.seed(initstr, sizeof(initstr));
	test2.seed(initstr, sizeof(initstr));
	ret_if_neq(test, test2, 1);

	// same result expected, as we only use the first 3 chars as seed.
	char initstr2[] = "abcqq";
	test.seed(initstr, 3);
	test2.seed(initstr2, 3);
	ret_if_neq(test, test2, 2);

	// test if seeding with nothing will fail.
	try {
		test.seed(nullptr, 0);
	}
	catch (util::Error &e) {
		return -1;
	}
	return 3;
}

/**
 * test the serializing functionality.
 */
static int rng_serialize() {
	// create seeded rng
	// and duplicate state to another one.
	RNG test{random_seed()};
	RNG test2{test.to_string()};
	ret_if_neq(test, test2, 0);

	try {
		test.from_string("100 aa");
		return 1;
		test.from_string("");
		return 2;
		test.from_string("100");
		return 3;
		test.from_string("100 ");
		return 4;
	}
	catch (util::Error) {
		// this error is expected!
		return -1;
	}
	return 5;
}

static int rng_real_fill() {
	constexpr size_t n = 1 << 7;
	RNG test{time_seed()};
	RNG test2{test};
	double data[n];

	test2.fill_real(data, n);
	for (size_t i = 0; i < n; i++) {
		if (test.real() != data[i]) {
			return 0;
		}
	}

	ret_if_neq(test, test2, 1);
	return -1;
}

/**
 * Tests filling n randoms.
 */
static int rng_fill() {
	constexpr size_t n = 1 << 7;
	uint64_t data[n];

	RNG test{random_seed()};
	RNG test2{test};

	test.fill(data, n);

	for (size_t i = 0; i < n; i++) {
		if (test2() != data[i]) {
			return 0;
		}
	}

	ret_if_neq(test, test2, 1);
	return -1;
}

/**
 * contains a test function and description of this test.
 */
struct data_test {
	int (*test_fnc)();
	const char *name;
};

static void perform_tests(std::vector<data_test> tests) {
	int ret;
	for (data_test test : tests) {
		if ((ret = test.test_fnc()) != -1) {
			log::log(MSG(err) << "Testing " << test.name << " failed at stage " << ret);
			throw "failed random number generator tests";
		}
	}
}

void rng_tests() {
	perform_tests({
		{rng_freq_dist, "the distribution of the generic generator"},
		{rng_bool_dist, "the distribution of the specialized bool generator"},
		{rng_real_dist, "the distribution of floating point numbers"},
		{rng_reproduce, "whether the rngs are reproducible"},
		{rng_serialize, "whether the rngs are serializable"},
		{rng_fill, "whether the fill function works properly"},
		{rng_real_fill, "the fill process for generating doubles"},
	});
}

}}} // namespace openage::rng::tests
