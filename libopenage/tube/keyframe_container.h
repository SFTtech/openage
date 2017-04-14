// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "tube.h"

#include "../error/error.h"

#include <cassert>
#include <iostream>
#include <limits>
#include <list>

namespace openage {
namespace tube {

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
class KeyframeContainer {
public:
	/**
     * A element of the double-linked list KeyframeContainer
	 */
	class Keyframe {
	public:
		Keyframe(const tube_time_t &time) :
			time(time) {}

		// Contruct it from time and value
		Keyframe(const tube_time_t &time, const _T &value) :
			time(time),
			value(value) {}

		const tube_time_t time = 0;
		_T value = _T();
	};

	typedef std::list<Keyframe> tubecontainer;
	typedef typename tubecontainer::const_iterator KeyframeIterator;
	KeyframeContainer();
	~KeyframeContainer();

	// Get the last element with e->time <= time
	KeyframeIterator last(const tube_time_t &time, const KeyframeIterator & hint) const;
	KeyframeIterator last(const tube_time_t &time) const {
		return this->last(time, this->container.begin());
	}

	/**
	 * Insert a new element without a hint.
	 *
	 * This function is recommended for use, whenever possible, keep a hint to insert
	 * the data.
	 */
	KeyframeIterator insert(const Keyframe &value) {
		return this->insert(value, this->container.begin());
	}

	/**
	 * Insert a new element. The hint shall give an approximate location, where the
	 * inserter will start to look for a insertion point. If a good hint is given, the
	 * runtime of this function will not be affected by the current history size.
	 */
	KeyframeIterator insert(const Keyframe &value, const KeyframeIterator &hint);

	/**
	 * Create and insert a new element without submitting a hint.
	 * The use of this function is discouraged, use it only, if your really do not
	 * have the possibility to get a hint
	 */
	KeyframeIterator insert(const tube_time_t &time, const _T&value) {
		return this->insert(Keyframe(time, value), this->container.begin());
	}
	/**
	 * Create and insert a new element. The hint gives an approximate location.
	 */
	KeyframeIterator insert(const tube_time_t &time, const _T&value, const KeyframeIterator &hint) {
		return this->insert(Keyframe(time, value), hint);
	}

	/**
	 * Erase all elements that come after this last valid element.
	 */
	KeyframeIterator erase_after(KeyframeIterator last_valid);

	/**
	 * Erase a single element from the tube.
	 */
	KeyframeIterator erase(KeyframeIterator );

	KeyframeIterator begin() const {
		return container.begin();
	}

	KeyframeIterator end() const {
		return container.end();
	}

	void __attribute__ ((noinline)) dump() {
		for (auto e : container) {
			std::cout << "Element: time: " << e.time << " v: " << e.value << std::endl;
		}
	}
private:
	tubecontainer container;
};


template<typename _T>
KeyframeContainer<_T>::KeyframeContainer() {
	//Create a default element at -Inf, that can always be dereferenced - so there will by definition never be
	//a element that cannot be dereferenced
	this->container.push_back(Keyframe(-std::numeric_limits<tube_time_t>::infinity(), _T()));
}

template<typename _T>
KeyframeContainer<_T>::~KeyframeContainer() {
}

/**
 * Select the element that directly preceedes the given timestamp.
 *
 * Without a hint, start to iterate at the beginning of the buffer, and return
 * the element last element before e->time > time.
 * This method returns nullptr, if begin->time > time.
 **/
template <typename _T>
typename KeyframeContainer<_T>::KeyframeIterator KeyframeContainer<_T>::last(const tube_time_t &time, const KeyframeIterator &hint) const {
	KeyframeIterator e = (hint == this->container.end()) ? this->container.begin() : hint;

	if (this->container.front().time > time) {
		// This will never happen due to the container.front->time == -Inf magic!
		throw new Error(ERR << "rupture in spacetime detected, tube container is broken");
	}

	// Search in the queue
	if (time > e->time) { // the searched element is supposed to be AFTER the hint
		// perform the search via ->next
		while (e != this->container.end() && time >= e->time) {
			e++;
		}
		e--;
		// e is now one of two options:
		// 1. e == end: The last element of the queue was smaller than `time`
		// 2. e != end: There was a element with `e->time` > `time`

	} else if (time < e->time) {
		// the searched element is supposed to be BEFORE the hint
		// perform the search via ->prev
		while (e != this->container.begin() && time < e->time) {
			e--;
		}
		// e is now one of two options:
		// 1. e == begin: The time was before every element in the queue
		// 2. e != begin: There was an element with `e->time` > `time`

	} else {
		// perform <= search - and return e, whose time is == time.
	}

	return e;
}

/**
 * Determine where to insert based on time, and insert
 */
template <typename _T>
typename KeyframeContainer<_T>::KeyframeIterator KeyframeContainer<_T>::insert(const KeyframeContainer<_T>::Keyframe &e, const KeyframeContainer<_T>::KeyframeIterator &hint) {
	KeyframeIterator at = this->last(e.time, hint);
	at ++;
	return this->container.insert(at, e);
}

/**
 * Go from the end to the last_valid element, and call erase on all of them
 */
template <typename _T>
typename KeyframeContainer<_T>::KeyframeIterator KeyframeContainer<_T>::erase_after(KeyframeContainer<_T>::KeyframeIterator last_valid) {
	//Delete from the end to (excluded) last_valid
	return this->container.erase(++last_valid, container.end());
}

/**
 * Delete the element from the list and call delete on it.
 */
template <typename _T>
typename KeyframeContainer<_T>::KeyframeIterator KeyframeContainer<_T>::erase(KeyframeContainer<_T>::KeyframeIterator e) {
	return this->container.erase(e);
}

}} // openage::tube
