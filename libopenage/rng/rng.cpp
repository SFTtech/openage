// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#include "rng.h"

#include <random>
#include <sstream>
#include <array>

#include "../util/timing.h"
#include "../util/hash.h"
#include "../error/error.h"


namespace openage {


/**
 * Contains an easy to use and fast random number generator.
 *
 * This was never designed or intended as a cryptographic-quality,
 * RNG, so don't use it as one. If you do, I'll tell my mom.
 */
namespace rng {


// Key for seed hashing, just some hardcoded key
static const std::array<uint8_t, 16> seed_key {{
	0xba, 0xda, 0x55, 0x00, 0x5e, 0xed, 0x00, 0x00,
	0xab, 0xcd, 0xef, 0x12, 0x34, 0x56, 0x78, 0x90
}};


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


void RNG::seed(const void *dat, size_t count) {
	if (count == 0) {
		throw Error(MSG(err) << "0 bytes supplied as seed data");
	}

	const auto *data = static_cast<const uint8_t *>(dat);

	openage::util::Siphash siphash(seed_key);

	state[0] = siphash.digest(data, count);
	// Previously, for state[1] we hashed (data ^ state[0])
	// As `data` now has variable length I can't do that, I think that just
	// hashing `state[0]` is enough
	state[1] = siphash.digest(state[0]);

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
		if (rnd.entropy() > 0) {
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
