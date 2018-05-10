// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <utility>

#include "../error/error.h"


namespace openage {
namespace datastructure {

/**
 * One entry in the lookup map.
 */
template<class K, class V>
class ConstMapEntry {
public:
	constexpr ConstMapEntry(const K &key, const V &value)
		:
		key{key},
		value{value} {}

	constexpr ConstMapEntry(std::pair<K, V> pair)
		:
		key{pair.first},
		value{pair.second} {}

	/**
	 * Map entry key.
	 */
	const K key;

	/**
	 * Map entry value.
	 */
	const V value;
};

/**
 * Compiletime generic lookup map.
 *
 * This is a recursive template that decreases `pos` which each
 * iteration step. Each `pos` stores one map entry.
 * The lookup recursively descends those entries down to the zero-element.
 *
 * If you experience compiler errors, make sure you request _existing_ keys.
 * We intentionally trigger compiler failures when a key doesn't exist.
 *
 * Messages include: "error: ‘*0u’ is not a constant expression"
 * -> nonexistant key
 */
template<typename K, typename V, size_t pos>
class ConstMap {
public:
	template<class Head, class... Tail>
	constexpr ConstMap(Head head, Tail... tail)
		:
		value{head},
		tail{tail...} {}

	/**
	 * Return the number of entries in this map.
	 */
	constexpr int inline size() const {
		return this->verify_no_duplicates(), pos;
	}

	/**
	 * Tests if the given key is in this map.
	 */
	constexpr bool inline contains(const K &key) const {
		return this->verify_no_duplicates(), this->has_entry(key);
	}

	/**
	 * Return the stored value for the given key.
	 */
	constexpr const inline V &get(const K &key) const {
		return this->verify_no_duplicates(), this->fetch(key);
	}

	/**
	 * Access entries by map[key].
	 */
	constexpr const inline V &operator [](const K &key) const {
		return this->get(key);
	}

private:
	/**
	 * Test if if the given key is stored in this element or in following elements.
	 */
	constexpr bool inline has_entry(const K &key) const {
		return (this->value.key == key) or this->tail.has_entry(key);
	}

	/**
	 * Test if the key of this entry is not used in following entries.
	 */
	constexpr bool inline is_unique() const {
		return not this->tail.has_entry(this->value.key);
	}

	/**
	 * Verifies that this entry's key is not used in following entries,
	 * then continues checking with the next entry checking for the same.
	 */
	constexpr bool inline all_are_unique() const {
		return this->is_unique() and this->tail.all_are_unique();
	}

	/**
	 * Abort when the map uses the same key more than once.
	 */
	constexpr bool inline verify_no_duplicates() const {
		return
		this->all_are_unique() ?
			true
		:
			throw Error(MSG(err) << "There is a duplicate key in the map.");
	}

	/**
	 * Returns the value matching key from this or following elements.
	 */
	constexpr const inline V &fetch(const K &key) const {
		return
		// if we found the key
		(this->value.key == key) ?
			// return the value
			this->value.value
		:
			// else try the lookup on the remaining elements
			this->tail.fetch(key);
	}

	/**
	 * The entry associated with this map position.
	 */
	const ConstMapEntry<K, V> value;

	/**
	 * The remaining map entries.
	 */
	const ConstMap<K, V, pos - 1> tail;

	/**
	 * The previous entry is our friend.
	 */
	friend class ConstMap<K, V, pos + 1>;
};


/**
 * constexpr map end container.
 * When this element is reached, all contained entries have been processed.
 *
 * For example, when searching a key and calling to this class means
 * the key was not found.
 */
template<typename K, typename V>
class ConstMap<K, V, 0> {
public:
	constexpr ConstMap() {}

	/**
	 * The size of the end element.
	 */
	constexpr int inline size() const {
		return 0;
	}

	/**
	 * The end element can't contain the key.
	 */
	constexpr bool inline contains(const K &) const {
		return false;
	}

private:
	/**
	 * Reaching the last entry of the uniqueness check means
	 * we had no duplicates.
	 */
	constexpr bool inline all_are_unique() const {
		return true;
	}

	/**
	 * The end element doesn't have an entry.
	 */
	constexpr bool inline has_entry(const K &) const {
		return false;
	}

	/**
	 * The end element doesn't have an entry,
	 * so nothing can be fetched,
	 */
	constexpr const inline V &fetch(const K &) const {
		// if we reach here recursively, the key was not found along the way and so isn't present
		throw Error(MSG(err) << "The key is not in the map.");
	}

	/**
	 * The previous entry class is our friend.
	 */
	friend class ConstMap<K, V, 1>;
};


/**
 * Creates a compiletime lookup table from
 * K to V, where all entries of K must be unique.
 *
 * usage: constexpr auto bla = create_const_map<type0, type1>(entry0, entry1, ...);
 */
template<typename K, typename V, typename... Entries>
constexpr ConstMap<K, V, sizeof...(Entries)> create_const_map(Entries&&... entry) {
	return ConstMap<K, V, sizeof...(entry)>(ConstMapEntry<K, V>{std::forward<Entries>(entry)}...);
}

}} // openage::datastructure
