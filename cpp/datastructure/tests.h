// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_DATASTRUCTURE_TESTS_H_
#define OPENAGE_DATASTRUCTURE_TESTS_H_

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


#endif
