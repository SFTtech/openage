// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "hash.h"

namespace openage {
namespace util {

size_t hash_combine(size_t hash1, size_t hash2) {
	// Taken from http://www.boost.org/doc/libs/1_55_0/doc/html/hash/reference.html#boost.hash_combine
	return hash1 ^ (hash2 + 0x9e3779b9 + ((hash1 << 6) + (hash1 >> 2)));
}

}} // openage::util
