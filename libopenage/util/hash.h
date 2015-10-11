// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_HASH_H_
#define OPENAGE_UTIL_HASH_H_

#include <cstdlib>

namespace openage {
namespace util {

/**
 * Creates a hash value as a combination of two other hashes. Can be called incrementally to create
 * hash value from several variables.
 */
size_t hash_combine(size_t hash1, size_t hash2);

}} // openage::util

#endif
