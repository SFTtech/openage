// Copyright 2015-2020 the openage authors. See copying.md for legal info.

#pragma once

#include <iostream>
#include <string>

namespace openage::rng {


/** \class RNG
 * This class holds an xorshift128+ rng, as well as
 * utility generator functions
 */
class RNG {
public:
	/**
	 * Creates an rng and seeds it with the 64 bit seed
	 * @param seed The inital seed for the following number generation.
	 */
	explicit RNG(uint64_t seed);

    /**
	 * Initializes the rng using data from the buffer pointed to by data
	 * @param data The buffer that contains data for seeding the rng
	 * @param count The number of bytes in the buffer
	 * @throws Error if 0 bytes are passed
	 */
	RNG(const void *data, size_t count);


	/**
	 * Reads the rng in from the passed string. If you want to
	 * use the data in the string as a seed, then pass the c_str() to
	 * rng(const void*, size_t).
	 * @param instr The string from which the rng is serialized
	 * @throws Error if the rng cannot be read from the string
	 */
	explicit RNG(const std::string &instr);


	/**
	 * Reads the rng in from the passed std::istream. This serializes
	 * the rng, and does not use the data as a random seed.
	 * @param instream The input stream for serializing the rng
	 * @throws Error if stream initialization fails
	 */
	explicit RNG(std::istream &instream);


	/**
	 * Seeds with the 64 bit seed
	 */
	void seed(uint64_t val);


	/**
	 * Initializes the rng using bits from the buffer pointed to by data
	 * @param data The buffer that contains data for seeding the rng
	 * @param count The number of bytes in the buffer
	 * @throws Error if 0 bytes are passed
	 */
	void seed(const void *data, size_t count);


	/**
	 * Retrieves a random value from the generator
	 */
	uint64_t random();


	/**
	 * Retrieves a random value from the generator
	 */
	inline uint64_t operator ()() {
		return this->random();
	}


	/**
	 * Returns a random integer in the range [lower, upper]
	 */
	inline uint64_t random_range(uint64_t lower, uint64_t upper) {
		return (this->random() % (upper - lower)) + lower;
	}


	/**
	 * Returns true with the passed probability
	 */
	inline bool probability(double prob) {
		return this->random() < (RNG::max() * prob);
	}


	/**
	 * Retrieves a double in [0, 1)
	 */
	inline double real() {
		return static_cast<double>(this->random()) / RNG::max();
	}


	/**
	 * Returns a double in the range [min, max)
	 */
	inline double real_range(double lower, double upper) {
		return this->real() * (upper - lower) + lower;
	}


	/**
	 * Effeciently fills a chunk of 64 bit integers.
	 * Gives identical result to calling random() len times
	 * @param data The buffer to be filled with random numbers
	 * @param len the length of the data
	 */
	void fill(uint64_t *data, size_t len);


	/**
	 * Fills an array of doubles with values in [0, 1)
	 * Gives identical results as real() but more effeciently
	 * @param data A pointer t0 the buffer of doubles
	 * @param len The length of the buffer
	 */
	void fill_real(double *data, size_t len);


	/**
	 * Discards num_discard numbers from the generator.
	 */
	void discard(size_t num_discard);


	/**
	 * Outputs the rng state to a stream
	 * @throws Error if writing data fails
	 */
	std::ostream &to_stream(std::ostream &out) const;


	/**
	 * Reads the rng state from a stream
	 * @throws Error if reading the stream fails
	 */
	std::istream &from_stream(std::istream &in);


	/**
	 * Writes the rng state to a string
	 */
	[[nodiscard]] std::string to_string() const;


	/**
	 * Reads the rng in from a string
	 * @throws Error if reading from the string fails
	 */
	void from_string(const std::string &instr);


	static constexpr uint64_t max() {
		return UINT64_MAX;
	}


	static constexpr uint64_t min() {
		return 0;
	}


private:
	/**
	 * The internal state array
	 */
	uint64_t state[2]{};


	/**
	 * elements to discard on seed to escape from high-zero seeds
	 */
	constexpr static size_t discard_on_seed = 50;
};


/**
 * Reads the rng from a stream
 * @throws Error if reading from the stream fails
 */
std::istream &operator >>(std::istream &instream, RNG &inrng);

/**
 * Writes the rng state to a stream
 * @throws Error if writing data fails
 */
std::ostream &operator <<(std::ostream &ostream, const RNG &inrng);


/**
 * Returns a seed based on time since epoch
 */
uint64_t time_seed();


/**
 * Returns a seed based on time since epoch
 * as well as data from std::random_device if available
 */
uint64_t random_seed();


} // namespace openage::rng
