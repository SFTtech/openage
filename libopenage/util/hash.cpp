// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include <array>
#include <limits.h>

#include "hash.h"
#include "misc.h"

namespace openage {
namespace util {

size_t hash_combine(size_t hash1, size_t hash2) {
	// Taken from http://www.boost.org/doc/libs/1_55_0/doc/html/hash/reference.html#boost.hash_combine
	return hash1 ^ (hash2 + 0x9e3779b9 + ((hash1 << 6) + (hash1 >> 2)));
}


Siphash::Siphash(std::array<uint8_t, 16> key) {
	set_key(key);
}


Siphash& Siphash::set_key(std::array<uint8_t, 16> key) {
	this->key[0] = array8_to_uint64(key.data(), 8, false);
	this->key[1] = array8_to_uint64(key.data() + 8, 8, false);

	return *this;
}


/**
 * Siphash implementation
 *
 * https://131002.net/siphash/
 */
uint64_t hash(const uint64_t key[2], const uint8_t *data, size_t len_data) {

	static auto sipround = [](uint64_t *v) {
		v[0] += v[1];
		v[1] = rol<uint64_t, 13>(v[1]);
		v[1] ^= v[0];
		v[0] = rol<uint64_t, 32>(v[0]);
		v[2] += v[3];
		v[3] = rol<uint64_t, 16>(v[3]);
		v[3] ^= v[2];
		v[0] += v[3];
		v[3] = rol<uint64_t, 21>(v[3]);
		v[3] ^= v[0];
		v[2] += v[1];
		v[1] = rol<uint64_t, 17>(v[1]);
		v[1] ^= v[2];
		v[2] = rol<uint64_t, 32>(v[2]);
	};

	// (len_data mod 256) ending to append later at the end of the data
	uint64_t ending = static_cast<uint64_t>(len_data & 0xff) << 56;

	// Initialization

	uint64_t v[4];
	// These values were taken from the paper: https://131002.net/siphash/siphash.pdf
	v[0] = key[0] ^ 0x736f6d6570736575ull;
	v[1] = key[1] ^ 0x646f72616e646f6dull;
	v[2] = key[0] ^ 0x6c7967656e657261ull;
	v[3] = key[1] ^ 0x7465646279746573ull;

	// Compression

	{
		size_t rem_bytes{len_data};
		const uint8_t *idx{data}; // Points to the start of the current block being copied
		uint64_t block;
		bool finished{false};

		do {
			if (rem_bytes < 8) {
				block = array8_to_uint64(idx, rem_bytes, false);
				block |= ending;
				finished = true;
			}
			else {
				block = array8_to_uint64(idx, 8, false);
			}

			v[3] ^= block;
			sipround(v);
			sipround(v);
			v[0] ^= block;

			rem_bytes -= 8;
			idx += 8;
		} while (!finished);
	}

	// Finalization

	v[2] ^= 0xff;
	sipround(v);
	sipround(v);
	sipround(v);
	sipround(v);

	return v[0] ^ v[1] ^ v[2] ^ v[3];
}


uint64_t Siphash::digest(const uint8_t *data, size_t count) {
	return hash(key, data, count);
}


uint64_t Siphash::digest(uint64_t value) {
	std::vector<uint8_t> data = uint64_to_array8(value, true);
	return hash(key, data.data(), 8);
}


}} // openage::util
