// Copyright 2015-2015 the openage authors. See copying.md for legal info.
#include "../log.h"
#include "../util/error.h"
#include "rng.h"

#include <limits>
#include <vector>
#include <cmath>

namespace openage {
namespace rng {
namespace tests {
constexpr size_t num_rand = 500000;

static int rng_freq_t(void) {
	constexpr size_t dsize = std::numeric_limits<unsigned char>::max() + 1;
	constexpr size_t mean = num_rand / dsize;
	constexpr size_t max_diff = mean * 0.1;
	size_t data_points[dsize];
	for(auto& dat : data_points) {
		dat = 0;
	}
	rng myrng(time_seed());
	for(size_t i = 0; i < num_rand/8; i++) {
		uint64_t val = myrng();
		unsigned char *mystuff = (unsigned char*)&val;
		for(size_t j = 0; j < sizeof(val); j++) {
			++data_points[mystuff[j]];
		}
	}
	for(size_t count : data_points) {
		if((size_t)std::abs((double)mean - count) > max_diff) {
			return 0;
		}
	}
	return -1;
}

static int rng_bool_t (void) {
	constexpr size_t expected = num_rand / 2;
	constexpr size_t max_diff = expected * 0.1;
	rng mybool(random_seed());
	size_t num_true = 0;
	for(size_t i = 0; i < num_rand; i++) {
		num_true = (mybool() & 1) ? num_true + 1 : num_true;
	}
	if((size_t)std::abs((double)num_true - expected) > max_diff) {
		return 0;
	}
	return -1;
}

#define ret_if_neq(rn1, rn2, val)	  \
	for(size_t qq = 0; qq < num_rand; qq++) \
		if(rn1() != rn2()) \
			return val;

static int rng_reproduce_t () {
	auto val = time_seed();
	rng test(val);
	rng test2(val);
	ret_if_neq(test, test2, 0);

	char initstr[] = "abcdefghijk";
	test.seed(initstr, sizeof(initstr));
	test2.seed(initstr, sizeof(initstr));
	ret_if_neq(test, test2, 1);

	char initstr2[] = "abcqq";
	test.seed(initstr, 3);
	test2.seed(initstr2, 3);
	ret_if_neq(test, test2, 2);

	try {
		test.seed(nullptr, 0);
	}
	catch (util::Error& e) {
		return -1;
	}
	return 3;
}

static int rng_serialize_t() {
	rng test(random_seed());
	rng test2(test.to_string());
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
	catch (util::Error) {}
	return -1;
}

static int rng_real_fill() {
	rng test(time_seed());
	rng test2(test);
	double data[101];
	test2.fill_real(data, 101);
	for(size_t i = 0; i < 101; i++) {
		if(test.real() != data[i]) {
			return 0;
		}
	}
	ret_if_neq(test, test2, 1);
	return -1;
}

static int rng_real_dist() {
	constexpr size_t dsize = 1000;
	constexpr size_t mean = num_rand / dsize;
	constexpr size_t max_diff = mean * 0.1;
	size_t data_points[dsize+1];
	for(auto& dat : data_points) {
		dat = 0;
	}
	rng myrng(time_seed());
	for(size_t i = 0; i < num_rand; i++) {
		double val = myrng.real();
		data_points[(int)(val * dsize)]++;
	}
	for(size_t i = 0; i < dsize; i++) {
		if((size_t)std::abs((double)mean - data_points[1]) > max_diff) {
			return 0;
		}
	}
	if(data_points[dsize]) {
		return 1;
	}
	return -1;

}
static int rng_fill() {
	uint64_t data[101];
	
	rng test(random_seed());
	rng test2(test);

	test.fill(data, 101);
	for(size_t i = 0; i < 101; i++) {
		if(test2() != data[i]) {
			return 0;
		}
	}
	ret_if_neq(test, test2, 1);
	return -1;
}

struct data_test {
	int (*test_fnc)();
	const char *name;
};
	
void perform_tests(std::vector<data_test> tests) {
	int ret;
	for(data_test test : tests) {
		if((ret = test.test_fnc()) != -1) {
			log::err("%s failed at stage %d", test.name, ret);
			throw "failed pairing heap tests";
		}
	}
}

void rng_tests() {
	perform_tests({
			{rng_bool_t, "Tests the distribution of the specialized bool generator"},
			{rng_freq_t, "Tests the distribution of the generic generator"},
			{rng_reproduce_t, "Tests whether the rngs are reproducible"},
			{rng_serialize_t, "Tests whether the rngs are serializable"},
			{rng_fill, "Test whether the fill function works properly"},
			{rng_real_fill, "Test the fill process for generating doubles"},
			{rng_real_dist, "Test the distribution of floating point numbers"},
		});
}

} //namespace tests
} //namespace rng
} //namespace openage
