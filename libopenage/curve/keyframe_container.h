// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <iostream>
#include <limits>
#include <list>

#include "curve.h"
#include "keyframe.h"
#include "../error/error.h"
#include "../util/compiler.h"

namespace openage::curve {

/**
 * A timely ordered list with several management functions
 *
 * This class manages different time-based management functions for list
 * approach that lies underneath. It contains list to be accessed via a
 * non-accurate timing functionality, this means, that for getting a value, not
 * the exact timestamp has to be known, it will always return the one closest,
 * less or equal to the requested one.
 **/
template <typename T>
class KeyframeContainer {
public:
	/**
	 * A element of the curvecontainer. This is especially used to keep track of
	 * the value-timing.
	 */
	using keyframe_t = Keyframe<T>;

	/**
	 * The underlaying container type.
	 *
	 * The most important property of this container is the iterator validity on
	 * insert and remove.
	 */
	using container_t = std::list<keyframe_t>;

	/**
	 * The iterator type to access elements in the container
	 */
	using iterator = typename container_t::const_iterator;
	using const_iterator = typename container_t::const_iterator;

	/** need the datamanger for change management **/
	KeyframeContainer();

	/**
	 * Return the number of elements in this container.
	 * One element is always added at -Inf by default,
	 * so this is usually your_added_elements + 1.
	 */
	size_t size() const;

	/**
	 * Get the last element in the curve which is before the given time.
	 * (i.e. elem->time <= time). Given a hint where to start the search.
	 */
	iterator last(const time_t &time,
	              const iterator & hint) const;

	/**
	 * Get the last element with elem->time <= time, without a hint where to start
	 * searching.
	 *
	 * The usage of this method is discouraged - except if there is absolutely
	 * no chance for you to have a hint (or the container is known to be nearly
	 * empty)
	 */
	iterator last(const time_t &time) const {
		return this->last(time, std::end(this->container));
	}

	/**
	 * Insert a new element without a hint.
	 *
	 * Starts the search for insertion at the end of the data.
	 * This function is not recommended for use, whenever possible, keep a hint
	 * to insert the data.
	 */
	iterator insert_before(const keyframe_t &value) {
		return this->insert_before(value, std::end(this->container));
	}

	/**
	 * Insert a new element. The hint shall give an approximate location, where
	 * the inserter will start to look for a insertion point. If a good hint is
	 * given, the runtime of this function will not be affected by the current
	 * history size. If there is a keyframe with identical time, this will
	 * insert the new keyframe before the old one.
	 */
	iterator insert_before(const keyframe_t &value, const iterator &hint);

	/**
	 * Create and insert a new element without submitting a hint. The search is
	 * started from the end of the data. The use of this function is
	 * discouraged, use it only, if your really do not have the possibility to
	 * get a hint.
	 */
	iterator insert_before(const time_t &time, const T &value) {
		return this->insert_before(keyframe_t(time, value), std::end(this->container));
	}

	/**
	 * Create and insert a new element. The hint gives an approximate location.
	 * If there is a value with identical time, this will insert the new value
	 * before the old one.
	 */
	iterator insert_before(const time_t &time, const T &value,
	                       const iterator &hint) {
		return this->insert_before(keyframe_t(time, value), hint);
	}

	/**
	 * Insert a new element, overwriting elements that have a
	 * time conflict. Give an approximate insertion location to minimize runtime
	 * on big-history curves.
	 * `overwrite_all` == true -> overwrite all same-time elements.
	 * `overwrite_all` == false -> overwrite the last of the time-conflict elements.
	 */
	iterator insert_overwrite(const keyframe_t &value,
	                          const iterator &hint,
	                          bool overwrite_all=false);

	/**
	 * Insert a new value at given time which will overwrite the last of the
	 * elements with the same time. This function will start to search the time
	 * from the end of the data. The use of this function is discouraged, use it
	 * only, if your really do not have the possibility to get a hint.
	 */
	iterator insert_overwrite(const time_t &time, const T &value) {
		return this->insert_overwrite(keyframe_t(time, value),
		                              std::end(this->container));
	}

	/**
	 * Insert a new value at given time, which overwrites element(s) with
	 * identical time. If `overwrite_all` is false, overwrite the last same-time
	 * element. If `overwrite_all` is true, overwrite all elements with same-time.
	 * Provide a insertion hint to abbreviate the search for the
	 * insertion point.
	 */
	iterator insert_overwrite(const time_t &time,
	                          const T &value,
	                          const iterator &hint,
	                          bool overwrite_all=false) {
		return this->insert_overwrite(keyframe_t(time, value), hint, overwrite_all);
	}

	/**
	 * Insert a new element, after a previous element when there's a time
	 * conflict. Give an approximate insertion location to minimize runtime on
	 * big-history curves.
	 */
	iterator insert_after(const keyframe_t &value, const iterator &hint);

	/**
	 * Insert a new value at given time which will be prepended to the block of
	 * elements that have the same time. This function will start to search the
	 * time from the end of the data. The use of this function is discouraged,
	 * use it only, if your really do not have the possibility to get a hint.
	 */
	iterator insert_after(const time_t &time, const T &value) {
		return this->insert_after(keyframe_t(time, value),
		                          std::end(this->container));
	}

	/**
	 * Create and insert a new element, which is added after a previous element with
	 * identical time. Provide a insertion hint to abbreviate the search for the insertion point.
	 */
	iterator insert_after(const time_t &time, const T &value, const iterator &hint) {
		return this->insert_after(keyframe_t(time, value), hint);
	}

	/**
	 * Erase all elements that come after this last valid element.
	 */
	iterator erase_after(iterator last_valid);

	/**
	 * Erase a single element from the curve.
	 * Returns the element after the deleted one.
	 */
	iterator erase(iterator it);

	/**
	 * Erase all elements with given time.
	 * Variant without hint, starts the search at the end of the container.
	 * Returns the iterator after the deleted elements.
	 */
	iterator erase(const time_t &time) {
		return this->erase(time, std::end(this->container));
	}

	/**
	 * Erase all element with given time.
	 * `hint` is an iterator pointing hopefully close to the searched
	 * elements.
	 *
	 * Returns the iterator after the deleted elements.
	 * Or, if no elements with this time exist,
	 * the iterator to the first element after the requested time is returned
	 */
	iterator erase(const time_t &time,
	               const iterator &hint) {
		return this->erase_group(time, this->last(time, hint));
	}

	/**
	 * Obtain an iterator to the first value with the smallest timestamp.
	 */
	iterator begin() const {
		return this->container.begin();
	}

	/**
	 * Obtain an iterator to the position after the last value.
	 */
	iterator end() const {
		return this->container.end();
	}

	/**
	 * Debugging method to be used from gdb to understand bugs better.
	 */
	void dump() const {
		for (auto &e : container) {
			std::cout << "Element: time: " << e.time << " v: " << e.value << std::endl;
		}
	}
private:

	/**
	 * Erase elements with this time.
	 * The iterator has to point to the last element of the same-time group.
	 */
	iterator erase_group(const time_t &time,
	                     const iterator &last_elem);

	/**
	 * The data store.
	 */
	container_t container;
};


template<typename T>
KeyframeContainer<T>::KeyframeContainer() {

	// Create a default element at -Inf, that can always be dereferenced - so
	// there will by definition never be a element that cannot be dereferenced
	this->container.push_back(keyframe_t(std::numeric_limits<time_t>::min(), T()));
}


template <typename T>
size_t KeyframeContainer<T>::size() const {
	return this->container.size();
}


/*
 * Select the last element that is <= a given time.
 * If there is multiple elements with the same time, return the last of them.
 * If there is no element with such time, return the next element before the time.
 *
 * Without a hint, start to iterate at the beginning of the buffer, and return
 * the element last element before e->time > time.
 *
 * Intuitively, this function returns the element that set the last value
 * that determines the curve value for a searched time.
 */
template <typename T>
typename KeyframeContainer<T>::iterator KeyframeContainer<T>::last(const time_t &time,
                                                                   const iterator &hint) const {

	iterator e = hint;
	auto end = std::end(this->container);

	if (e != end and e->time <= time) {
		// walk to the right until the time is larget than the searched
		// then go one to the left to get the last item with <= requested time
		while (e != end && e->time <= time) {
			e++;
		}
		e--;
	}
	else { // e == end or e->time > time
		// walk to the left until the element time is smaller or equal the searched time
		auto begin = std::begin(this->container);
		while (e != begin and (e == end or e->time > time)) {
			e--;
		}
	}

	return e;
}


/*
 * Determine where to insert based on time, and insert.
 */
template <typename T>
typename KeyframeContainer<T>::iterator
KeyframeContainer<T>::insert_before(const KeyframeContainer<T>::keyframe_t &e,
                                    const KeyframeContainer<T>::iterator &hint) {
	iterator at = this->last(e.time, hint);
	// seek over all same-time elements, so we can insert before the first one
	while (at != std::begin(this->container) and at->time == e.time) {
		--at;
	}

	// the above while-loop overshoots and selects the first non-equal element.
	// set iterator one to the right, i.e. on the first same-value
	if (at != std::end(this->container)) {
		++at;
	}
	return this->container.insert(at, e);
}


/*
 * Determine where to insert based on time, and insert, overwriting value(s) with same time.
 */
template <typename T>
typename KeyframeContainer<T>::iterator
KeyframeContainer<T>::insert_overwrite(
	const KeyframeContainer<T>::keyframe_t &e,
	const KeyframeContainer<T>::iterator &hint,
	bool overwrite_all) {

	iterator at = this->last(e.time, hint);

	if (overwrite_all) {
		at = this->erase_group(e.time, at);
	}
	else if (at != std::end(this->container)) {
		// overwrite the same-time element
		if (at->time == e.time) {
			at = this->container.erase(at);
		}
		else {
			++at;
		}
	}
	return this->container.insert(at, e);
}


/*
 * Determine where to insert based on time, and insert.
 * If there is a time conflict, insert after the existing element.
 */
template <typename T>
typename KeyframeContainer<T>::iterator
KeyframeContainer<T>::insert_after(
	const KeyframeContainer<T>::keyframe_t &e,
	const KeyframeContainer<T>::iterator &hint) {

	iterator at = this->last(e.time, hint);

	if (at != std::end(this->container)) {
		++at;
	}
	return this->container.insert(at, e);
}


/*
 * Go from the end to the last_valid element, and call erase on all of them
 */
template <typename T>
typename KeyframeContainer<T>::iterator
KeyframeContainer<T>::erase_after(KeyframeContainer<T>::iterator last_valid) {

	// exclude the last_valid element from deletion
	if (last_valid != this->container.end()) {
		++last_valid;
	}

	// Delete everything to the end.
	while (last_valid != this->container.end()) {
		last_valid = this->container.erase(last_valid);
	}
	return last_valid;
}


/*
 * Delete the element from the list and call delete on it.
 */
template <typename T>
typename KeyframeContainer<T>::iterator
KeyframeContainer<T>::erase(KeyframeContainer<T>::iterator e) {
	return this->container.erase(e);
}


template <typename T>
typename KeyframeContainer<T>::iterator
KeyframeContainer<T>::erase_group(const time_t &time,
                                  const iterator &last_elem) {

	iterator at = last_elem;

	// if the time what we're looking for
	// erase elements until all element with that time are purged
	while (at != std::end(this->container) and at->time == time) {
		at = this->container.erase(at);
		if (likely(at != std::begin(this->container))) {
			--at;
		}
	}

	// we have to cancel one --at in order to return
	// the element after the group we deleted.
	if (at != std::end(this->container)) {
		++at;
	}

	return at;
}

} // openage::curve
