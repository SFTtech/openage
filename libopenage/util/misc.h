// Copyright 2013-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <limits.h>
#include <cmath>
#include <cstring>
#include <string>
#include <vector>

#include "../error/error.h"


namespace openage {
namespace util {

/**
 * global empty string, sometimes needed
 * as return value in some `std::string &function()` return values.
 */
extern std::string empty_string;


/**
 * modulo operation that guarantees to return positive values.
 */
template <typename T>
constexpr
T mod(T x, T m) {
	T r = x % m;

	if (r < 0) {
		return r + m;
	} else {
		return r;
	}
}

/**
 * compiletime defined modulo function.
 */
template <typename T, unsigned int modulo>
constexpr
T mod(T x) {
	T r = x % modulo;

	if (r < 0) {
		return r + modulo;
	} else {
		return r;
	}
}


/**
 * compiletime defined rotate left function
 */
template <typename T, int amount>
constexpr
T rol(T x) {
	static_assert(sizeof(T)*CHAR_BIT > amount && amount > 0, "invalid rotation amount");
	return (x << amount) | (x >> (sizeof(T)*CHAR_BIT - amount));
}


/**
 * implements the 'correct' version of the division operator,
 * which always rounds to -inf
 */
template <typename T>
constexpr
inline T div(T x, T m) {
	return (x - mod<T>(x, m)) / m;
}

/**
 * generic callable, that compares any types for creating a total order.
 *
 * use for stdlib structures like std::set.
 * the template parameter has to be a pointer type.
 */
template <typename T>
struct less {
	bool operator ()(const T x, const T y) const {
		return *x < *y;
	}
};


// Size of uint64_t measured in bytes
static constexpr size_t uint64_s = 8;


/**
 * Convert a C-style array of uint8_t to a uint64_t. Uses big-endianness as
 * default.
 * Fills the remaining bytes with zeroes.
 *
 * e.g (big-endian): {0x01, 0x23, 0x45, 0x67} -> 0x0123456700000000
 * e.g (little-endian): {0x01, 0x23, 0x45, 0x67} -> 0x0000000067452301
 *
 * @param start Pointer to start of the input data.
 * @param count Number of bytes to read.
 * @param big_endian Endianness of byte array.
 * @return Input data as a 64 bit number.
 */
inline uint64_t
array8_to_uint64(const uint8_t *start, size_t count, bool big_endian=true) {
	if (count > uint64_s) {
		throw Error(MSG(err) << "Tried to copy more than " << uint64_s << " bytes");
	}

	uint64_t result{0};
	for (size_t i = 0; i < count; i++) {
		if (big_endian) {
			result |= static_cast<uint64_t>(*(start + i)) << (7 - i) * 8;
		}
		else {
			result |= static_cast<uint64_t>(*(start + i)) << i * 8;
		}
	}
	return result;
}


/**
 * Convert a uint64_t to an array of uint8_t. Uses big-endianness as default.
 *
 * e.g (big-endian):
 *     0x0123456789abcdef -> {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef}
 * e.g (little-endian):
 *     0x0123456789abcdef -> {0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01}
 *
 * @param value 64 bit number to convert.
 * @param big_endian Endianness of byte array.
 * @return Input data as a 8 bit number array.
 */
inline std::vector<uint8_t>
uint64_to_array8(const uint64_t value, bool big_endian=true) {
	std::vector<uint8_t> result(uint64_s, 0);

	for (size_t i = 0; i < uint64_s; i++) {
		if (big_endian) {
			result.at(i) = (value >> ((7 - i) * 8)) & 0xff;
		}
		else {
			result.at(i) = (value >> (i * 8)) & 0xff;
		}
	}
	return result;
}


/**
 * Get return array size for array8_to_array64()
 *
 * @return ceil(count/8)
 */
inline constexpr size_t array64_size(size_t count) {
	return std::ceil(static_cast<double>(count) / static_cast<double>(uint64_s));
}


/**
 * Convert a C-style array of uint8_t to a std::vector of uint64_t.
 * Uses big-endianness as default.
 * Fills the remaining bytes of the last uint64_t with zeroes.
 *
 * e.g (big-endian): {0x01, 0x23, 0x45, 0x67} -> 0x0123456700000000
 * e.g (little-endian): {0x01, 0x23, 0x45, 0x67} -> 0x0000000067452301
 *
 * @param start Start of the input data.
 * @param count Number of bytes to convert.
 * @param big_endian Endianness of byte array.
 * @return Input data as a 64 bit number vector.
 */
inline std::vector<uint64_t>
array8_to_array64(const uint8_t *start, size_t count, bool big_endian=true) {
	size_t size{array64_size(count)};
	std::vector<uint64_t> result(size, 0);

	size_t rem_bytes;
	for (size_t i = 0; i < size; i++) {
		rem_bytes = count - i * uint64_s;
		result.at(i) = array8_to_uint64(start + (i * uint64_s),
		                                std::min(rem_bytes, uint64_s),
		                                big_endian);
	}
	return result;
}


/**
 * Convert a C-style array of uint64_t to a std::vector of uint8_t.
 * Uses big-endianness ad default.
 *
 * e.g (big-endian):
 *     0x0123456789abcdef -> {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef}
 * e.g (little-endian):
 *     0x0123456789abcdef -> {0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01}
 *
 * @param Start of the input data.
 * @param count Number of uint64_t to convert.
 * @param big_endian Endianness of byte array.
 * @return Input data as a 8 bit number vector.
 */
inline std::vector<uint8_t>
array64_to_array8(const uint64_t *start, size_t count, bool big_endian=true) {
	std::vector<uint8_t> result;
	result.reserve(count * uint64_s);

	for (size_t i = 0; i < count; i++) {
		std::vector<uint8_t> block = uint64_to_array8(*(start + i), big_endian);
		result.insert(result.end(), block.begin(), block.end());
	}

	return result;
}


/**
 * Extend a vector with elements, without destroying source one.
 */
template <typename T>
void vector_extend(std::vector<T> &vec, const std::vector<T> &ext) {
	vec.reserve(vec.size() + ext.size());
	vec.insert(std::end(vec), std::begin(ext), std::end(ext));
}


/*
 * Extend a vector with elements with move semantics.
 */
template <typename T>
void vector_extend(std::vector<T> &vec, std::vector<T> &&ext) {
	if (vec.empty()) {
		vec = std::move(ext);
	}
	else {
		vec.reserve(vec.size() + ext.size());
		std::move(std::begin(ext), std::end(ext), std::back_inserter(vec));
		ext.clear();
	}
}


/**
 * Remove the given element index in the vector.
 * May swap with the end element for efficient removing.
 *
 * If the element is not in the vector, do nothing.
 */
template <typename T>
void vector_remove_swap_end(std::vector<T> &vec, size_t idx) {
	// is at the end
	if (idx == vec.size() - 1) {
		vec.pop_back();
	}
	// is in the middle
	else if (idx < vec.size()) {
		std::swap(vec[idx], vec.back());
		vec.pop_back();
	} else {
		return;
	}
}


} // namespace util
} // namespace openage
