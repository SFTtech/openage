// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <array>
#include <type_traits>
#include <utility>

#include "../error/error.h"


namespace openage::datastructure {

/**
 * Compiletime generic lookup map.
 *
 * Stores the map entries in an array and uses constexpr methods
 * to search and retrieve them at compile-time.
 *
 * If you experience compiler errors, make sure you request _existing_ keys.
 * We intentionally trigger compiler failures when a key doesn't exist.
 *
 * Messages include: "error: ‘*0u’ is not a constant expression"
 * -> nonexistant key
 */
template<typename K, typename V, size_t count>
class ConstMap {
public:
	template<class... Entries>
	constexpr ConstMap(Entries&&... entries)
		:
		values{std::forward<Entries>(entries)...} {
		this->verify_no_duplicates();
	}

	/**
	 * Return the number of entries in this map.
	 */
	constexpr int size() const {
		return count;
	}

	/**
	 * Tests if the given key is in this map.
	 */
	constexpr bool contains(const K &key) const {
		return this->find(key) != values.end();
	}

	/**
	 * Return the stored value for the given key.
	 */
	constexpr const V &get(const K &key) const {
		auto iter = this->find(key);
		if (iter == values.end()) {
			throw Error(MSG(err) << "The key is not in the map.");
		}

		return iter->second;
	}

	/**
	 * Access entries by map[key].
	 */
	constexpr const V &operator [](const K &key) const {
		return this->get(key);
	}

private:
	/**
	 * Abort when the map uses the same key more than once.
	 */
	constexpr void verify_no_duplicates() const {
		for (auto iter1 = values.begin(); iter1 != values.end(); ++iter1) {
			for (auto iter2 = iter1 + 1; iter2 != values.end(); ++iter2) {
				if (iter1->first == iter2->first) {
					throw Error(MSG(err) << "There is a duplicate key in the map.");
				}
			}
		}
	}

	/**
	 * Returns the iterator matching key from the array.
	 *  - `values.end()` if the key is not found.
	 */
	constexpr auto find(const K &key) const {
		for (auto iter = values.begin(); iter != values.end(); ++iter) {
			if (iter->first == key) {
				return iter;
			}
		}
		return values.end();
	}

	/**
	 * The entries associated with this map.
	 */
	std::array<std::pair<K, V>, count> values;
};

/**
 * Specialization for size 0.
 * Needed until https://bugs.llvm.org/show_bug.cgi?id=40124 is resolved.
 */
template<typename K, typename V>
class ConstMap<K, V, 0> {
public:
	/**
	 * Empty map has 0 size.
	 */
	constexpr int size() const {
		return 0;
	}

	/**
	 * Empty map contains no key/value pairs.
	 */
	constexpr bool contains(const K &) const {
		return false;
	}
};

/**
 * Creates a compiletime lookup table from
 * K to V, where all entries of K must be unique.
 *
 * usage: constexpr auto bla = create_const_map<type0, type1>(entry0, entry1, ...);
 */
template<typename K, typename V, typename... Entries>
constexpr auto create_const_map(Entries&&... entry) {
	return ConstMap<K, V, sizeof...(entry)>{entry...};
}

/**
 * Template deduction guide to deduce the Key-Value types
 * for the ConstMap from the paired entries passed.
 *
 * usage: constexpr ConstMap boss{std::pair{k0, v0}, std::pair{k1, v1}, ...};
 *
 * Note: Use when automatic type deduction is desirable.
 *       For manually specifying types, use the other method.
 */
template<typename Entry, typename... Rest,
         typename = std::enable_if_t<std::conjunction_v<std::is_same<Entry, Rest>...>>>
ConstMap(Entry, Rest&&...) -> ConstMap<typename Entry::first_type,
                                       typename Entry::second_type,
                                       1 + sizeof...(Rest)>;

} // openage::datastructure
