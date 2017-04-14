// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <unordered_map>

#include "tube.h"
#include "map_filter_iterator.h"

namespace openage {
namespace tube {

template <typename key_t, typename val_t>
class UnorderedMap {
	struct map_element {
		val_t value;
		tube_time_t alive;
		tube_time_t dead;
		map_element (val_t v, const tube_time_t &a, const tube_time_t &d)
			: value(v),
			  alive(a),
			  dead(d) {}
	};
	std::unordered_map<key_t, map_element> container;

public:
	// Using does not work with templates
	typedef typename std::unordered_map<key_t, map_element>::iterator iterator;
// TODO return an std::optional here.
	std::pair<bool, TubeMapFilterIterator<key_t, val_t, UnorderedMap>> operator()(const tube_time_t&, const key_t &);
// TODO return an std::optional here.
	std::pair<bool, TubeMapFilterIterator<key_t, val_t, UnorderedMap>> at(const tube_time_t &, const key_t &);

	TubeMapFilterIterator<key_t, val_t, UnorderedMap> begin(const tube_time_t &e = std::numeric_limits<tube_time_t>::infinity());
	TubeMapFilterIterator<key_t, val_t, UnorderedMap> end(const tube_time_t &e = std::numeric_limits<tube_time_t>::infinity());


	TubeMapFilterIterator<key_t, val_t, UnorderedMap> insert(const key_t &, const val_t &);
	TubeMapFilterIterator<key_t, val_t, UnorderedMap> insert(const tube_time_t &birth, const key_t &, const val_t &);
	TubeMapFilterIterator<key_t, val_t, UnorderedMap> insert(const tube_time_t &birth, const tube_time_t &death, const key_t &key, const val_t &value);

	TubeMapFilterIterator<key_t, val_t, UnorderedMap> between(const tube_time_t &start, const tube_time_t &to);

	void birth(const tube_time_t &, const key_t &);
	void birth(const tube_time_t &, const TubeMapFilterIterator<val_t, val_t, UnorderedMap> &);

	void kill(const tube_time_t &, const key_t &);
	void kill(const tube_time_t &, const TubeMapFilterIterator<val_t, val_t, UnorderedMap> &);

	bool is_alive(const tube_time_t &, const key_t &);
	bool is_alive(const tube_time_t &, const TubeMapFilterIterator<val_t, val_t, UnorderedMap> &);

	void clean(const tube_time_t &); // remove all dead elements before that point in time

	void __attribute__((noinline)) dump() {
		for (auto i : container) {
			std::cout << "Element: " << i.second.value << std::endl;;
		}
	}
};

template<typename key_t, typename val_t>
std::pair<bool, TubeMapFilterIterator<key_t, val_t, UnorderedMap<key_t, val_t>>>
UnorderedMap<key_t, val_t>::operator()(const tube_time_t &time,
                                       const key_t &key) {
	return this->at(time, key);
}

template<typename key_t, typename val_t>
std::pair<bool, TubeMapFilterIterator<key_t, val_t, UnorderedMap<key_t, val_t>>>
UnorderedMap<key_t, val_t>::at(const tube_time_t & time,
                               const key_t & key) {
	auto e = this->container.find(key);
	if (e != this->container.end() && e->second.alive <= time && e->second.dead >time) {
		return std::make_pair(
			true,
			TubeMapFilterIterator<key_t, val_t, UnorderedMap<key_t, val_t>>(
				e,
				this->container.end(),
				time,
				std::numeric_limits<tube_time_t>::infinity()));
	} else {
		return std::make_pair(
			false,
			this->end(time));
	}
}

template<typename key_t, typename val_t>
TubeMapFilterIterator<key_t, val_t, UnorderedMap<key_t, val_t>>
UnorderedMap<key_t, val_t>::begin(const tube_time_t &time) {
	return TubeMapFilterIterator<key_t, val_t, UnorderedMap<key_t, val_t>>(
		this->container.begin(),
		this->container.end(),
		time,
		std::numeric_limits<tube_time_t>::infinity());
}

template<typename key_t, typename val_t>
TubeMapFilterIterator<key_t, val_t, UnorderedMap<key_t, val_t>>
UnorderedMap<key_t, val_t>::end(const tube_time_t &time) {
	return TubeMapFilterIterator<key_t, val_t, UnorderedMap<key_t, val_t>>(
		this->container.end(),
		this->container.end(),
		-std::numeric_limits<tube_time_t>::infinity(),
		time);
}

template<typename key_t, typename val_t>
TubeMapFilterIterator<key_t, val_t, UnorderedMap<key_t, val_t>>
UnorderedMap<key_t, val_t>::between(const tube_time_t &from, const tube_time_t &to) {
	auto it = TubeMapFilterIterator<key_t, val_t, UnorderedMap<key_t, val_t>>(
		this->container.begin(),
		this->container.end(),
		from,
		to);

	if (!it.valid(from)) {
		++it;
	}
	return it;
}

template<typename key_t, typename val_t>
TubeMapFilterIterator<key_t, val_t, UnorderedMap<key_t, val_t>>
UnorderedMap<key_t, val_t>::insert(const key_t &key,
                                   const val_t &value) {
	return this->insert(
		existent_from(value),
		existent_until(value),
		key,
		value);
}

template<typename key_t, typename val_t>
TubeMapFilterIterator<key_t, val_t, UnorderedMap<key_t, val_t>>
UnorderedMap<key_t, val_t>::insert(const tube_time_t &alive,
                                   const key_t &key,
                                   const val_t &value) {
	return this->insert(
		alive,
		std::numeric_limits<tube_time_t>::infinity(),
		key,
		value);
}

template<typename key_t, typename val_t>
TubeMapFilterIterator<key_t, val_t, UnorderedMap<key_t, val_t>>
UnorderedMap<key_t, val_t>::insert(const tube_time_t &alive,
                                   const tube_time_t &dead,
                                   const key_t &key,
                                   const val_t &value) {
	map_element e(value, alive, dead);
	auto it = this->container.insert(std::make_pair(key, e));
	return TubeMapFilterIterator<key_t, val_t, UnorderedMap<key_t, val_t>>(
		it.first,
		this->container.end(),
		alive,
		dead);
}

template<typename key_t, typename val_t>
void UnorderedMap<key_t, val_t>::birth(const tube_time_t &time,
                                       const key_t &key) {
	auto it = this->container.find(key);
	if (it != this->container.end()) {
		it->second.alive = time;
	}
}

template<typename key_t, typename val_t>
void UnorderedMap<key_t, val_t>::birth(const tube_time_t &time,
                                       const TubeMapFilterIterator<val_t, val_t,
                                       UnorderedMap> &it) {
	it->second.alive = time;
}

template<typename key_t, typename val_t>
void UnorderedMap<key_t, val_t>::kill(const tube_time_t &time,
                                      const key_t &key) {
	auto it = this->container.find(key);
	if (it != this->container.end()) {
		it->second.dead = time;
	}
}

template<typename key_t, typename val_t>
void UnorderedMap<key_t, val_t>::kill(const tube_time_t &time,
                                      const TubeMapFilterIterator<val_t, val_t, UnorderedMap> &it) {
	it->second.dead = time;
}

template<typename key_t, typename val_t>
bool UnorderedMap<key_t, val_t>::is_alive(const tube_time_t &time,
                                          const key_t &key) {
	auto it = this->container.find(key);
	if (it != this->container.end()) {
		return valid_f(it->second.value, time);
	}
}

template<typename key_t, typename val_t>
bool UnorderedMap<key_t, val_t>::is_alive(const tube_time_t &time,
                                          const TubeMapFilterIterator<val_t, val_t,
                                          UnorderedMap> &it) {
	return valid_f(it->second.value, time);
}

template<typename key_t, typename val_t>
void UnorderedMap<key_t, val_t>::clean(const tube_time_t &) {
	// TODO save everything to a file and be happy.
}

}} // openage::tube
