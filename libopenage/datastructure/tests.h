// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <stddef.h>

namespace openage {
namespace datastructure {
namespace tests {

/**
 * simplest priority queue element that supports reordering.
 */
struct heap_elem {
	int data;

	bool operator <(const heap_elem &other) const {
		return this->data < other.data;
	}

	bool operator ==(const heap_elem &other) const {
		return this->data == other.data;
	}
};

} // namespace tests
} // namespace datastructure
} // namespace openage

namespace std {

/**
 * hash function for the simple heap_elem
 */
template<>
struct hash<openage::datastructure::tests::heap_elem> {
	size_t operator ()(const openage::datastructure::tests::heap_elem &elem) const {
		return elem.data;
	}
};
} // namespace std
