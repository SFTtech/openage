// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "rng.h"

#include <random>
#include <sstream>
#include <string.h>

#include "../crossplatform/timing.h"
#include "../error/error.h"


namespace openage {


/**
 * Contains an easy to use and fast random number generator.
 *
 * This was never designed or intended as a cryptographic-quality,
 * RNG, so don't use it as one. If you do, I'll tell my mom.
 */
namespace rng {


RNG::RNG(uint64_t v1) {
	this->seed(v1);
}


RNG::RNG(const void *data, size_t len) {
	this->seed(data, len);
}


RNG::RNG(const std::string &instr) {
	this->from_string(instr);
}


RNG::RNG(std::istream &instream) {
	this->from_stream(instream);
}


void RNG::seed(uint64_t val) {
	this->seed(&val, sizeof(val));
}


/**
 * murmurhash3 bit avalancher - output is uniformly distributed
 * and changing one bit of input results in ~1/2 of output bits changed
 * This function is a good way of mixing up an input,
 * or mixing together two results with hash64(val1 ^ val2)
 *
 * https://en.wikipedia.org/wiki/MurmurHash
 */
static uint64_t hash64(uint64_t val) {
	// The two magic numbers have been determined heuristically.
	// They avalanche all bits in val to within 0.25% bias.

	val ^= val >> 33;
	val *= 0xff51afd7ed558ccd;
	val ^= val >> 33;
	val *= 0xc4ceb9fe1a85ec53;
	return val ^ (val >> 33);
}


/*
 * reinterpret_casts 8 chars from a buffer as a uint64_t.
 */
static inline uint64_t from_cptr(const char *data) {
	return *reinterpret_cast<const uint64_t *>(data);
}


/*
 * hashes an initial seed into the base state
 */
static void init_hash(uint64_t *state, uint64_t seed) {
	// Mix the initial seed into data for each state value
	state[0] = hash64(seed);
	state[1] = hash64(seed ^ state[0]);
}


/*
 * For each 8-byte block after the initial seed,
 * mix it with state[0] and set state[0] equal to the result.
 * Then, set state[1] to the mixture of state[1] and state[0]
 * This lets each bit from the input play a role in the output
 * w/o requiring that the size be a multiple of 8 or having extra
 * code for the non multiple-of-8 sections
 */
static void hash_bytes(uint64_t *state, const char *data, size_t count) {
	for (size_t i = 1; i <= count; i++) {
		state[0] = hash64(state[0] ^ from_cptr(data+i));
		state[1] = hash64(state[0] ^ state[1]);
	}
}


void RNG::seed(const void *dat, size_t count) {
	if (count == 0) {
		throw Error(MSG(err) << "0 bytes supplied as seed data");
	}

	constexpr uint64_t default_seed = 0x0123456789abcdef;
	constexpr size_t uint64_s = sizeof(uint64_t);
	const char *data = static_cast<const char *>(dat);

	if (count > uint64_s) {
		init_hash(state, from_cptr(data));
		hash_bytes(state, data, count - uint64_s);
	}
	else {
		uint64_t init_seed = 0;
		memcpy(&init_seed, data, count);
		init_hash(state, init_seed);
	}

	if (!(state[0] || state[1])) {
		state[0] = default_seed;
	}

	// This helps prevent low-zero states caused by a bad seed
	this->discard(discard_on_seed);
}


/*
 * actually does the number crunching and state updates
 * http://en.wikipedia.org/wiki/Xorshift
 */
static inline uint64_t do_rng(uint64_t &s0, uint64_t &s1) {
	s1 ^= s1 << 23;
	s1 = (s1 ^ s0 ^ (s1 >> 17) ^ (s0 >> 26));
	return s1 + s0;
}


uint64_t RNG::random() {
	std::swap(this->state[0], this->state[1]);
	return do_rng(this->state[0], this->state[1]);
}


/*
 * Provides serious performance optimizations over the standard
 * generator by eliminating memory writes and data dependencies.
 */
template<class T, class Lambda>
static inline void act_fill(T *dat, uint64_t *state,
                            size_t len, const Lambda &op) {

	uint64_t s0 = state[0];
	uint64_t s1 = state[1];
	size_t ev_len = len - (len & 1);

	for (size_t i = 0; i < ev_len; i += 2) {
		op(do_rng(s1, s0), dat, i);
		op(do_rng(s0, s1), dat, i + 1);
	}
	if (len & 1) {
		op(do_rng(s1, s0), dat, len-1);
		std::swap(s1, s0);
	}
	state[0] = s0;
	state[1] = s1;
}


void RNG::fill(uint64_t *dat, size_t len) {
	act_fill<>(
		dat, this->state, len,
		[](uint64_t v, uint64_t *d, size_t i) {
			d[i] = v;
		}
	);
}


void RNG::discard(size_t len) {
	act_fill<uint64_t>(
		nullptr, this->state, len,
		[](uint64_t, uint64_t *, size_t) {}
	);
}


void RNG::fill_real(double *dat, size_t len) {
	act_fill(
		dat, this->state, len,
		[](double v, double *d, size_t i) {
			d[i] = v / RNG::max();
		}
	);
}


std::ostream &RNG::to_stream(std::ostream &out) const {
	if ((out << this->state[0] << " ").good() &&
	    !(out << this->state[1]).fail()) {
		return out;
	}
	throw Error(MSG(err) << "Unable to write rng state serialization to output stream");
}


std::istream &RNG::from_stream(std::istream &in) {
	if ((in >> this->state[0]).good() &&
	    !(in >> this->state[1]).fail()) {
		return in;
	}
	throw Error(MSG(err) << "Unable to read rng state serialization from input stream");
}


std::string RNG::to_string() const {
	util::FString fs;
	fs << *this;
	return fs;
}


void RNG::from_string(const std::string &instr) {
	std::stringstream strstr(instr);
	strstr >> *this;
}


std::ostream &operator <<(std::ostream &ostream, const RNG &inrng) {
	inrng.to_stream(ostream);
	return ostream;
}


std::istream &operator >>(std::istream &instream, RNG &inrng) {
	inrng.from_stream(instream);
	return instream;
}


static uint64_t try_random_device() {
	uint64_t rand1 = 0;
	// std::random_device constructor may throw if it isn't available
	try {
		std::random_device rnd;
		if (rnd.entropy() > 0){
			rand1 = rnd() + (uint64_t(rnd()) << 32);
		}
	}
	catch (std::exception &e) {
		throw Error(MSG(err) << "failed using std::random_device: " << e.what());
	}
	return rand1;
}


static uint64_t time_seed(bool randomize) {
	uint64_t nanos = timing::get_real_time();

	if (randomize) {
		nanos ^= try_random_device();
	}
	return nanos;
}


uint64_t time_seed() {
	return time_seed(false);
}


uint64_t random_seed() {
	return time_seed(true);
}


}} // namespace openage::rng
