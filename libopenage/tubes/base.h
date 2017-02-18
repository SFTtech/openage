// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "tube.h"

#include <iostream>
#include <limits>
#include <cassert>

namespace openage {
namespace tube {

template <typename _T>
class tubeelement;

/**
 * A timely ordered list with several management functions
 *
 * This class manages different time-based management functions for the double-
 * linked list approach that lies underneath. It contains a double-linked list
 * to be accessed via a non-accurate timing functionality, this means, that for
 * getting a value, not the exact timestamp has to be known, it will always return
 * the one closest, less or equal to the requested one.
 **/
template <typename _T>
class tubebase {
	tubeelement<_T> *begin = nullptr;
	tubeelement<_T> *end = nullptr;

	// TODO create a pool where it might be possible to draw memory from
public:
	tubebase();

	~tubebase();

	// Get the last element with e->time <= time
	tubeelement<_T> *last(const tube_time_t &time, tubeelement<_T>* hint=nullptr) const;

	// Create a new element and insert it into the tree
	tubeelement<_T> *create(const tube_time_t &, const _T& value);

	// Insert a newly ekement into the tree, that has not yet been inserted.
	void insert(tubeelement<_T> *, tubeelement<_T> *hint = nullptr);

	// Erase the whole list after this element until the end.
	void erase_after(tubeelement<_T> *last_valid);

	// Remove the tubeelement from its container
	void erase(tubeelement<_T> *e);
};

/**
 >* A element of the double-linked list tubebase
 */
template <typename _T>
class tubeelement {
	friend class tubebase<_T>;
public:
	tubeelement *next = nullptr;
	tubeelement *prev = nullptr;
private:
	// These folks are for tubebase only!
	tubeelement(const tube_time_t &time)
		:
		time(time) {}

	// Contruct it from time and value
	tubeelement(const tube_time_t &time, const _T &value) :
		time(time),
		value(value)
		{}

public:
	const tube_time_t time = 0;
	_T value = _T();
};


template<typename _T>
tubebase<_T>::tubebase() {
	//Create a default element at -Inf, that can always be dereferenced - so there will by definition never be
	//a element that cannot be dereferenced
	create(-std::numeric_limits<tube_time_t>::infinity(), _T());
}

template<typename _T>
tubebase<_T>::~tubebase() {
	erase_after(begin);
	delete(begin);
}

/**
 * Select the element that directly preceedes the given timestamp.
 *
 * Without a hint, start to iterate at the beginning of the buffer, and return
 * the element last element before e->time > time.
 * This method returns nullptr, if begin->time > time.
 **/
template <typename _T>
tubeelement<_T> *tubebase<_T>::last(const tube_time_t &time, tubeelement<_T> *hint) const {
	tubeelement<_T> *e = hint ? hint : begin;

	if (e == nullptr) {
		//throw Error(ERR << "Empty container list!");
		return e;
	}

	if (begin->time > time) {
		// This will never happen due to the begin->time == -Inf magic!
		assert(false);
		return nullptr;
	}

	// Search in the queue
	if (time > e->time) { // the searched element is supposed to be AFTER the hint
		// perform the search via ->next
		while (e->next != nullptr && time >= e->next->time) {
			e = e->next;
		}
		// e is now one of two options:
		// 1. e == end: The last element of the queue was smaller than `time`
		// 2. e != end: There was a element with `e->time` > `time`
	} else {
		// the searched element is supposed to be BEFORE the hint
		// perform the search via ->prev
		while (e->prev != nullptr && time < e->time) {
			e = e->prev;
		}
		// e is now one of two options:
		// 1. e == begin: The time was before every element in the queue
		// 2. e != begin: There was an element with `e->time` > `time`
	}

	return e;
}

/**
 * Create and insert a new element into this tube
 */
template <typename _T>
tubeelement<_T> *tubebase<_T>::create(const tube_time_t &time, const _T& value) {
	// TODO this has to be managed by a memory pool!
	auto e = new tubeelement<_T>(time, value);
	insert(e);
	return e;
}

/**
 * Determine where to insert, and update all references
 */
template <typename _T>
void tubebase<_T>::insert(tubeelement<_T> *e, tubeelement<_T> *hint) {
	// There are no elements in the list right now.
	if (begin == nullptr) {
		begin = e;
		end = e;
		return;
	}

	tubeelement<_T>* at = last(e->time, hint);

	// if "last" cannot point at a location, so there was no element _before_
	// the newly inserted
	if (at == nullptr) {
		begin->prev = e;
		e->next = begin;
		begin = e;
	} else if (at->next == nullptr || end == at) {
		// if next is nullptr, then it has to be at the end, so update the end
		at->next = e;
		e->prev = at;
		end = e;
	} else {
		// the list is not empty, it is not at the beginning, it is not at the end:
		// it has to be in the middle! so we can perform a normal insert
		e->next = at->next;
		e->prev = at;
		at->next->prev = e;
		at->next = e;
	}
}


/**
 * Go from the end to the last_valid element, and call erase on all of them
 */
template <typename _T>
void tubebase<_T>::erase_after(tubeelement<_T> *last_valid) {
	tubeelement<_T> *e = end;
	//Delete from the end to last_valid
	while (e != nullptr && e != last_valid) {
		tubeelement<_T> *prev = e->prev;
		erase(e);
		e = prev;
	}
}

/**
 * Delete the element from the list and call delete on it.
 */
template <typename _T>
void tubebase<_T>::erase(tubeelement<_T> *e) {
	if (e == nullptr) return;
	if (begin == e) {
		begin = e->next;
	}
	if (end == e) {
		end = e->prev;
	}

	if (e->next != nullptr) {
		e->next->prev = e->prev;
	}
	if (e->prev != nullptr) {
		e->prev->next = e->next;
	}

	delete e; // TODO Memory management magic!
}

}} // openage::tube
