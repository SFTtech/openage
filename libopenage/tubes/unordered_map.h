// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <unordered_map>

#include "base.h"
#include "tube.h"
#include "filter_iterator.h"

namespace openage {
namespace tube {

template <typename key_t, typename val_t>
class UnorderedTubeMap {
	struct map_element {
	key_t key;
		tubebase<val_t> value;
		tube_time_t alive;
		tube_time_t dead;
	};
	std::unordered_map<key_t, map_element> container;

public:
	val_t &operator()(const tube_time_t&, const key_t &);
	val_t &at(const tube_time_t &, const key_t &);

	tube_filter_iterator<key_t, val_t, UnorderedTubeMap> begin(const tube_time_t &);

	tube_filter_iterator<key_t, val_t, UnorderedTubeMap> end(const tube_time_t &);

	tube_iterator<val_t> insert(const tube_time_t &, const key_t &, const val_t &);
	tube_iterator<val_t> insert(const tube_iterator<val_t> &at, const key_t &, const val_t &);

	void birth(const key_t &, const tube_time_t &);
	void birth(const tube_iterator<val_t> &, const tube_time_t &);

	void kill(const key_t &, const tube_time_t &);
	void kill(const tube_iterator<val_t> &, const tube_time_t &);

	void is_alive(const key_t &, const tube_time_t &);
	void is_alive(const tube_iterator<val_t> &, const tube_time_t &);

	void clean(const tube_time_t &); // remove all dead elements
};

template<typename key_t, typename val_t>
val_t &UnorderedTubeMap<key_t, val_t>::operator()(const tube_time_t &, const key_t &) {

}

template<typename key_t, typename val_t>
val_t &UnorderedTubeMap<key_t, val_t>::at(const tube_time_t &, const key_t &) {

}

template<typename key_t, typename val_t>
tube_filter_iterator<key_t, val_t, UnorderedTubeMap<key_t, val_t>> UnorderedTubeMap<key_t, val_t>::begin(const tube_time_t &) {

}

template<typename key_t, typename val_t>
tube_filter_iterator<key_t, val_t, UnorderedTubeMap<key_t, val_t>> UnorderedTubeMap<key_t, val_t>::end(const tube_time_t &) {

}

template<typename key_t, typename val_t>
tube_iterator<val_t> UnorderedTubeMap<key_t, val_t>::insert(const tube_time_t &, const key_t &, const val_t &) {

}

template<typename key_t, typename val_t>
tube_iterator<val_t> UnorderedTubeMap<key_t, val_t>::insert(const tube_iterator<val_t> &at, const key_t &, const val_t &) {

}

template<typename key_t, typename val_t>
void UnorderedTubeMap<key_t, val_t>::birth(const key_t &, const tube_time_t &) {

}

template<typename key_t, typename val_t>
void UnorderedTubeMap<key_t, val_t>::birth(const tube_iterator<val_t> &, const tube_time_t &) {

}

template<typename key_t, typename val_t>
void UnorderedTubeMap<key_t, val_t>::kill(const key_t &, const tube_time_t &) {

}

template<typename key_t, typename val_t>
void UnorderedTubeMap<key_t, val_t>::kill(const tube_iterator<val_t> &, const tube_time_t &) {

}

template<typename key_t, typename val_t>
void UnorderedTubeMap<key_t, val_t>::is_alive(const key_t &, const tube_time_t &) {

}

template<typename key_t, typename val_t>
void UnorderedTubeMap<key_t, val_t>::is_alive(const tube_iterator<val_t> &, const tube_time_t &) {

}

template<typename key_t, typename val_t>
void UnorderedTubeMap<key_t, val_t>::clean(const tube_time_t &) {

}

}} // openage::tube
