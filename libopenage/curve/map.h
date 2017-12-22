// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "curve.h"
#include "map_filter_iterator.h"

#include <iostream>
#include <unordered_map>

namespace openage {
namespace curve {

/**
 * Map that keeps track of the lifetime of the contained elements.
 * make shure, that no key is never reused.
 */
template <typename key_t, typename val_t>
class UnorderedMap {
	/** Internal container to access all data and metadata */
	struct map_element {
		val_t value;
		curve_time_t alive;
		curve_time_t dead;
		map_element (val_t v, const curve_time_t &a, const curve_time_t &d)
			: value(v),
			  alive(a),
			  dead(d) {}
	};
	/** the magic is inside here */
	std::unordered_map<key_t, map_element> container;

public:
	// Using does not work with templates
	typedef typename std::unordered_map<key_t, map_element>::const_iterator const_iterator;

	// TODO return an std::optional here
	std::pair<bool, MapFilterIterator<key_t, val_t, UnorderedMap>>
	operator()(const curve_time_t&, const key_t &) const;

	// TODO return an std::optional here.
	std::pair<bool, MapFilterIterator<key_t, val_t, UnorderedMap>>
	at(const curve_time_t &, const key_t &) const;

	MapFilterIterator<key_t, val_t, UnorderedMap>
	begin(const curve_time_t &e = std::numeric_limits<curve_time_t>::max()) const;

	MapFilterIterator<key_t, val_t, UnorderedMap>
	end(const curve_time_t &e = std::numeric_limits<curve_time_t>::max()) const;

	MapFilterIterator<key_t, val_t, UnorderedMap>
	insert(const curve_time_t &birth, const key_t &, const val_t &);

	MapFilterIterator<key_t, val_t, UnorderedMap>
	insert(const curve_time_t &birth, const curve_time_t &death, const key_t &key, const val_t &value);

	MapFilterIterator<key_t, val_t, UnorderedMap>
	between(const curve_time_t &start, const curve_time_t &to) const;

	void birth(const curve_time_t &, const key_t &);
	void birth(const curve_time_t &,
	           const MapFilterIterator<val_t, val_t, UnorderedMap> &);

	void kill(const curve_time_t &, const key_t &);
	void kill(const curve_time_t &,
	          const MapFilterIterator<val_t, val_t, UnorderedMap> &);

	// remove all dead elements before that point in time
	void clean(const curve_time_t &);

	void __attribute__((noinline)) dump() {
		for (auto i : container) {
			std::cout << "Element: " << i.second.value << std::endl;;
		}
	}
};

template<typename key_t, typename val_t>
std::pair<bool, MapFilterIterator<key_t, val_t, UnorderedMap<key_t, val_t>>>
UnorderedMap<key_t, val_t>::operator()(const curve_time_t &time,
                                       const key_t &key) const {
	return this->at(time, key);
}

template<typename key_t, typename val_t>
std::pair<bool, MapFilterIterator<key_t, val_t, UnorderedMap<key_t, val_t>>>
UnorderedMap<key_t, val_t>::at(const curve_time_t & time,
                               const key_t & key) const {
	auto e = this->container.find(key);
	if (e != this->container.end() && e->second.alive <= time && e->second.dead >time) {
		return std::make_pair(
			true,
			MapFilterIterator<key_t, val_t, UnorderedMap<key_t, val_t>>(
				e,
				this,
				time,
				std::numeric_limits<curve_time_t>::max()));
	} else {
		return std::make_pair(
			false,
			this->end(time));
	}
}

template<typename key_t, typename val_t>
MapFilterIterator<key_t, val_t, UnorderedMap<key_t, val_t>>
UnorderedMap<key_t, val_t>::begin(const curve_time_t &time) const {
	return MapFilterIterator<key_t, val_t, UnorderedMap<key_t, val_t>>(
		this->container.begin(),
		this,
		time,
		std::numeric_limits<curve_time_t>::max());
}

template<typename key_t, typename val_t>
MapFilterIterator<key_t, val_t, UnorderedMap<key_t, val_t>>
UnorderedMap<key_t, val_t>::end(const curve_time_t &time) const {
	return MapFilterIterator<key_t, val_t, UnorderedMap<key_t, val_t>>(
		this->container.end(),
		this,
		-std::numeric_limits<curve_time_t>::max(),
		time);
}

template<typename key_t, typename val_t>
MapFilterIterator<key_t, val_t, UnorderedMap<key_t, val_t>>
UnorderedMap<key_t, val_t>::between(const curve_time_t &from, const curve_time_t &to) const {
	auto it = MapFilterIterator<key_t, val_t, UnorderedMap<key_t, val_t>>(
		this->container.begin(),
		this,
		from,
		to);

	if (!it.valid()) {
		++it;
	}
	return it;
}

template<typename key_t, typename val_t>
MapFilterIterator<key_t, val_t, UnorderedMap<key_t, val_t>>
UnorderedMap<key_t, val_t>::insert(const curve_time_t &alive,
                                   const key_t &key,
                                   const val_t &value) {
	return this->insert(
		alive,
		std::numeric_limits<curve_time_t>::max(),
		key,
		value);
}

template<typename key_t, typename val_t>
MapFilterIterator<key_t, val_t, UnorderedMap<key_t, val_t>>
UnorderedMap<key_t, val_t>::insert(const curve_time_t &alive,
                                   const curve_time_t &dead,
                                   const key_t &key,
                                   const val_t &value) {
	map_element e(value, alive, dead);
	auto it = this->container.insert(std::make_pair(key, e));
	return MapFilterIterator<key_t, val_t, UnorderedMap<key_t, val_t>>(
		it.first,
		this,
		alive,
		dead);
}

template<typename key_t, typename val_t>
void UnorderedMap<key_t, val_t>::birth(const curve_time_t &time,
                                       const key_t &key) {
	auto it = this->container.find(key);
	if (it != this->container.end()) {
		it->second.alive = time;
	}
}

template<typename key_t, typename val_t>
void UnorderedMap<key_t, val_t>::birth(const curve_time_t &time,
                                       const MapFilterIterator<val_t, val_t,
                                       UnorderedMap> &it) {
	it->second.alive = time;
}

template<typename key_t, typename val_t>
void UnorderedMap<key_t, val_t>::kill(const curve_time_t &time,
                                      const key_t &key) {
	auto it = this->container.find(key);
	if (it != this->container.end()) {
		it->second.dead = time;
	}
}

template<typename key_t, typename val_t>
void UnorderedMap<key_t, val_t>::kill(const curve_time_t &time,
                                      const MapFilterIterator<val_t, val_t, UnorderedMap> &it) {
	it->second.dead = time;
}

template<typename key_t, typename val_t>
void UnorderedMap<key_t, val_t>::clean(const curve_time_t &) {
	// TODO save everything to a file and be happy.
}

}} // openage::curve
