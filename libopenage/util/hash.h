// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdlib>
#include <array>
#include <typeindex>

namespace openage {
namespace util {

/**
 * Helper function to generate hash of certain type T
 */

template <typename T>
size_t type_hash() {
    return std::hash<std::type_index>()(std::type_index(typeid(T)));
}

/**
 * A hash combination function
 * Creates a hash value as a combination of two other hashes. Can be called incrementally to create
 * hash value from several variables. Will always produce the same result for the same combination of
 * hash1 and hash2 during a single run of a program.
 */
size_t hash_combine(size_t hash1, size_t hash2);



/** \class Siphash
 * Contains a Siphash implementration.
 *
 * https://131002.net/siphash/
 */
class Siphash {
public:
	/**
	 * Creates a hash generator.
	 * @param key Key to use with this hasher.
	 */
	Siphash(std::array<uint8_t, 16> key);


	/**
	 * Set the key for subsequent hashes.
	 * @param k Key to use with this hasher.
	 * @return Reference to itself, for method chaining.
	 */
	Siphash& set_key(std::array<uint8_t, 16> key);


	/**
	 * Hashes the input data as uint8_t array and returns the result.
	 * @param data Start of the input data.
	 * @param len_data Number of bytes to read.
	 * @return Hash.
	 */
	uint64_t digest(const uint8_t *data, size_t len_data);


	/**
	 * Hashes a single uint64_t number and returns the result.
	 *
	 * The 64-bit word is interpreted as big-endian.
	 * e.g. 0x0123456789abcdef -> {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef}
	 *
	 * @param value uint64_t to hash.
	 * @return Hash.
	 */
	uint64_t digest(const uint64_t);


private:
	/**
	 * The key to use for hashing. As two 64-bit little-endian words.
	 */
	uint64_t key[2];
};


}} // openage::util
