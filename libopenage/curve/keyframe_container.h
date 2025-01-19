// Copyright 2017-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <functional>
#include <iostream>
#include <list>

#include "curve/keyframe.h"
#include "time/time.h"
#include "util/fixed_point.h"


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
	 */
	using container_t = std::vector<keyframe_t>;

	/**
	 * The index type to access elements in the container
	 */
	using elem_ptr = typename container_t::size_type;

	/**
	 * The iterator type to access elements in the container
	 */
	using iterator = typename container_t::const_iterator;

	/**
	 * Create a new container.
	 *
	 * Inserts a default element with value \p T() at \p time = -INF to ensure
	 * that accessing the container always returns an element.
	 *
	 * TODO: need the datamanger for change management
	 */
	KeyframeContainer();

	/**
	 * Create a new container.
	 *
	 * Inserts a default element at \p time = -INF to ensure
	 * that accessing the container always returns an element.
	 *
	 * @param defaultval Value of default element at -INF.
	 *
	 * TODO: need the datamanger for change management
	 */
	KeyframeContainer(const T &defaultval);

	/**
	 * Return the number of elements in this container.
	 * One element is always added at -Inf by default,
	 * so this is usually your_added_elements + 1.
	 */
	size_t size() const;

	const keyframe_t &get(const elem_ptr &idx) const {
		return this->container.at(idx);
	}

	/**
	 * Get the last element in the curve which is at or before the given time.
	 * (i.e. elem->time <= time). Given a hint where to start the search.
	 */
	elem_ptr last(const time::time_t &time,
	              const elem_ptr &hint) const;

	/**
	 * Get the last element with elem->time <= time, without a hint where to start
	 * searching.
	 *
	 * The usage of this method is discouraged - except if there is absolutely
	 * no chance for you to have a hint (or the container is known to be nearly
	 * empty)
	 */
	elem_ptr last(const time::time_t &time) const {
		return this->last(time, this->container.size());
	}

	/**
	 * Get the last element in the curve which is before the given time.
	 * (i.e. elem->time < time). Given a hint where to start the search.
	 */
	elem_ptr last_before(const time::time_t &time,
	                     const elem_ptr &hint) const;

	/**
	 * Get the last element with elem->time < time, without a hint where to start
	 * searching.
	 */
	elem_ptr last_before(const time::time_t &time) const {
		return this->last_before(time, this->container.size());
	}

	/**
	 * Insert a new element without a hint.
	 *
	 * Starts the search for insertion at the end of the data.
	 * This function is not recommended for use, whenever possible, keep a hint
	 * to insert the data.
	 */
	elem_ptr insert_before(const keyframe_t &value) {
		return this->insert_before(value, this->container.size());
	}

	/**
	 * Insert a new element. The hint shall give an approximate location, where
	 * the inserter will start to look for a insertion point. If a good hint is
	 * given, the runtime of this function will not be affected by the current
	 * history size. If there is a keyframe with identical time, this will
	 * insert the new keyframe before the old one.
	 */
	elem_ptr insert_before(const keyframe_t &value, const elem_ptr &hint);

	/**
	 * Create and insert a new element without submitting a hint. The search is
	 * started from the end of the data. The use of this function is
	 * discouraged, use it only, if your really do not have the possibility to
	 * get a hint.
	 */
	elem_ptr insert_before(const time::time_t &time, const T &value) {
		return this->insert_before(keyframe_t(time, value), this->container.size());
	}

	/**
	 * Create and insert a new element. The hint gives an approximate location.
	 * If there is a value with identical time, this will insert the new value
	 * before the old one.
	 */
	elem_ptr insert_before(const time::time_t &time, const T &value, const elem_ptr &hint) {
		return this->insert_before(keyframe_t(time, value), hint);
	}

	/**
	 * Insert a new element, overwriting elements that have a
	 * time conflict. Give an approximate insertion location to minimize runtime
	 * on big-history curves.
	 * `overwrite_all` == true -> overwrite all same-time elements.
	 * `overwrite_all` == false -> overwrite the last of the time-conflict elements.
	 */
	elem_ptr insert_overwrite(const keyframe_t &value,
	                          const elem_ptr &hint,
	                          bool overwrite_all = false);

	/**
	 * Insert a new value at given time which will overwrite the last of the
	 * elements with the same time. This function will start to search the time
	 * from the end of the data. The use of this function is discouraged, use it
	 * only, if your really do not have the possibility to get a hint.
	 */
	elem_ptr insert_overwrite(const time::time_t &time, const T &value) {
		return this->insert_overwrite(keyframe_t(time, value),
		                              this->container.size());
	}

	/**
	 * Insert a new value at given time, which overwrites element(s) with
	 * identical time. If `overwrite_all` is false, overwrite the last same-time
	 * element. If `overwrite_all` is true, overwrite all elements with same-time.
	 * Provide a insertion hint to abbreviate the search for the
	 * insertion point.
	 */
	elem_ptr insert_overwrite(const time::time_t &time,
	                          const T &value,
	                          const elem_ptr &hint,
	                          bool overwrite_all = false) {
		return this->insert_overwrite(keyframe_t(time, value), hint, overwrite_all);
	}

	/**
	 * Insert a new element, after a previous element when there's a time
	 * conflict. Give an approximate insertion location to minimize runtime on
	 * big-history curves.
	 */
	elem_ptr insert_after(const keyframe_t &value, const elem_ptr &hint);

	/**
	 * Insert a new value at given time which will be prepended to the block of
	 * elements that have the same time. This function will start to search the
	 * time from the end of the data. The use of this function is discouraged,
	 * use it only, if your really do not have the possibility to get a hint.
	 */
	elem_ptr insert_after(const time::time_t &time, const T &value) {
		return this->insert_after(keyframe_t(time, value),
		                          this->container.size());
	}

	/**
	 * Create and insert a new element, which is added after a previous element with
	 * identical time. Provide a insertion hint to abbreviate the search for the insertion point.
	 */
	elem_ptr insert_after(const time::time_t &time, const T &value, const elem_ptr &hint) {
		return this->insert_after(keyframe_t(time, value), hint);
	}

	/**
	 * Erase all elements that come after this last valid element.
	 */
	elem_ptr erase_after(elem_ptr last_valid);

	/**
	 * Erase a single element from the curve.
	 * Returns the element after the deleted one.
	 */
	elem_ptr erase(elem_ptr it);

	/**
	 * Erase all elements with given time.
	 * Variant without hint, starts the search at the end of the container.
	 * Returns the iterator after the deleted elements.
	 */
	elem_ptr erase(const time::time_t &time) {
		return this->erase(time, this->container.size());
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
	elem_ptr erase(const time::time_t &time,
	               const elem_ptr &hint) {
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
	 * Remove all keyframes from the container, EXCEPT for the default value
	 * at -INF.
	 *
	 * Essentially, the container is reset to the state immediately after construction.
	 */
	void clear() {
		this->container.erase(++this->begin(), this->end());
	}

	/**
	 * Copy keyframes from another container to this container.
	 *
	 * Replaces all keyframes beginning at t >= start with keyframes from \p other.
	 *
	 * @param other Curve that keyframes are copied from.
	 * @param start Start time at which keyframes are replaced (default = -INF).
	 *              Using the default value replaces ALL keyframes of \p this with
	 *              the keyframes of \p other.
	 */
	elem_ptr sync(const KeyframeContainer<T> &other,
	              const time::time_t &start = time::TIME_MIN);

	/**
	 * Copy keyframes from another container (with a different element type) to this container.
	 *
	 * Replaces all keyframes beginning at t >= start with keyframes from \p other.
	 *
	 * @param other Curve that keyframes are copied from.
	 * @param converter Function that converts the value type of \p other to the
	 *                  value type of \p this.
	 * @param start Start time at which keyframes are replaced (default = -INF).
	 *              Using the default value replaces ALL keyframes of \p this with
	 *              the keyframes of \p other.
	 */
	template <typename O>
	elem_ptr sync(const KeyframeContainer<O> &other,
	              const std::function<T(const O &)> &converter,
	              const time::time_t &start = time::TIME_MIN);

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
	elem_ptr erase_group(const time::time_t &time,
	                     const elem_ptr &last_elem);

	/**
	 * The data store.
	 */
	container_t container;
};


template <typename T>
KeyframeContainer<T>::KeyframeContainer() {
	// Create a default element at -Inf, that can always be dereferenced - so
	// there will by definition never be a element that cannot be dereferenced
	this->container.push_back(keyframe_t(time::TIME_MIN, T()));
}


template <typename T>
KeyframeContainer<T>::KeyframeContainer(const T &defaultval) {
	// Create a default element at -Inf, that can always be dereferenced - so
	// there will by definition never be a element that cannot be dereferenced
	this->container.push_back(keyframe_t(time::TIME_MIN, defaultval));
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
 * Intuitively, this function returns the element that set the last value
 * that determines the curve value for a searched time.
 */
template <typename T>
typename KeyframeContainer<T>::elem_ptr
KeyframeContainer<T>::last(const time::time_t &time,
                           const KeyframeContainer<T>::elem_ptr &hint) const {
	elem_ptr at = hint;
	const elem_ptr end = this->container.size();

	if (at != end and this->container.at(at).time() <= time) {
		// walk to the right until the time is larget than the searched
		while (at != end and this->container.at(at).time() <= time) {
			++at;
		}
		// go one back, because we want the last element that is <= time
		--at;
	}
	else { // idx == end or idx->time > time
		// walk to the left until the element time is smaller than the searched time
		while (at > 0 and (at == end or this->container.at(at).time() > time)) {
			--at;
		}
	}

	return at;
}


/*
 * Select the last element that is < a given time.
 * If there is multiple elements with the same time, return the last of them.
 * If there is no element with such time, return the next element before the time.
 *
 * Intuitively, this function returns the element that comes right before the
 * first element that matches the search time.
 */
template <typename T>
typename KeyframeContainer<T>::elem_ptr
KeyframeContainer<T>::last_before(const time::time_t &time,
                                  const KeyframeContainer<T>::elem_ptr &hint) const {
	elem_ptr at = hint;
	const elem_ptr end = this->container.size();

	if (at != end and this->container.at(at).time() < time) {
		// walk to the right until the time is larget than the searched
		while (at != end and this->container.at(at).time() <= time) {
			++at;
		}
		// go one back, because we want the last element that is <= time
		--at;
	}
	else { // idx == end or idx->time > time
		// walk to the left until the element time is smaller than the searched time
		while (at > 0 and (at == end or this->container.at(at).time() >= time)) {
			--at;
		}
	}

	return at;
}


/*
 * Determine where to insert based on time, and insert.
 */
template <typename T>
typename KeyframeContainer<T>::elem_ptr
KeyframeContainer<T>::insert_before(const KeyframeContainer<T>::keyframe_t &e,
                                    const KeyframeContainer<T>::elem_ptr &hint) {
	elem_ptr at = this->last(e.time(), hint);

	if (at == this->container.size()) {
		this->container.push_back(e);
		return at;
	}

	// seek over all same-time elements, so we can insert before the first one
	while (this->container.at(at).time() == e.time() and at > 0) {
		at--;
	}

	++at;

	this->container.insert(this->begin() + at, e);
	return at;
}


/*
 * Determine where to insert based on time, and insert, overwriting value(s) with same time.
 */
template <typename T>
typename KeyframeContainer<T>::elem_ptr
KeyframeContainer<T>::insert_overwrite(const KeyframeContainer<T>::keyframe_t &e,
                                       const KeyframeContainer<T>::elem_ptr &hint,
                                       bool overwrite_all) {
	elem_ptr at = this->last(e.time(), hint);
	const elem_ptr end = this->container.size();

	if (overwrite_all) {
		at = this->erase_group(e.time(), at);
	}
	else if (at != end) {
		// overwrite the same-time element
		if (this->get(at).time() == e.time()) {
			this->container.erase(this->begin() + at);
		}
		else {
			++at;
		}
	}

	this->container.insert(this->begin() + at, e);
	return at;
}


/*
 * Determine where to insert based on time, and insert.
 * If there is a time conflict, insert after the existing element.
 */
template <typename T>
typename KeyframeContainer<T>::elem_ptr
KeyframeContainer<T>::insert_after(const KeyframeContainer<T>::keyframe_t &e,
                                   const KeyframeContainer<T>::elem_ptr &hint) {
	elem_ptr at = this->last(e.time(), hint);
	const elem_ptr end = this->container.size();

	if (at != end) {
		++at;
	}

	this->container.insert(this->begin() + at, e);
	return at;
}


/*
 * Go from the end to the last_valid element, and call erase on all of them
 */
template <typename T>
typename KeyframeContainer<T>::elem_ptr
KeyframeContainer<T>::erase_after(KeyframeContainer<T>::elem_ptr last_valid) {
	// exclude the last_valid element from deletion
	if (last_valid != this->container.size()) {
		// Delete everything to the end.
		const elem_ptr delete_start = last_valid + 1;
		this->container.erase(this->begin() + delete_start, this->end());
	}

	return last_valid;
}


/*
 * Delete the element from the list and call delete on it.
 */
template <typename T>
typename KeyframeContainer<T>::elem_ptr
KeyframeContainer<T>::erase(KeyframeContainer<T>::elem_ptr e) {
	this->container.erase(this->begin() + e);
	return e;
}


template <typename T>
typename KeyframeContainer<T>::elem_ptr
KeyframeContainer<T>::sync(const KeyframeContainer<T> &other,
                           const time::time_t &start) {
	// Delete elements after start time
	elem_ptr at = this->last_before(start, this->container.size());
	at = this->erase_after(at);

	auto at_other = 1; // always skip the first element (because it's the default value)

	// Copy all elements from other with time >= start
	for (size_t i = at_other; i < other.size(); i++) {
		if (other.get(i).time() >= start) {
			at = this->insert_after(other.get(i), at);
		}
	}

	return this->container.size();
}


template <typename T>
template <typename O>
typename KeyframeContainer<T>::elem_ptr
KeyframeContainer<T>::sync(const KeyframeContainer<O> &other,
                           const std::function<T(const O &)> &converter,
                           const time::time_t &start) {
	// Delete elements after start time
	elem_ptr at = this->last_before(start, this->container.size());
	at = this->erase_after(at);

	auto at_other = 1; // always skip the first element (because it's the default value)

	// Copy all elements from other with time >= start
	for (size_t i = at_other; i < other.size(); i++) {
		if (other.get(i).time() >= start) {
			at = this->insert_after(keyframe_t(other.get(i).time(), converter(other.get(i).val())), at);
		}
	}

	return this->container.size();
}


template <typename T>
typename KeyframeContainer<T>::elem_ptr
KeyframeContainer<T>::erase_group(const time::time_t &time,
                                  const KeyframeContainer<T>::elem_ptr &last_elem) {
	size_t at = last_elem;

	// if the time what we're looking for
	// erase elements until all element with that time are purged
	while (at != this->container.size() and this->container.at(at).time() == time) {
		this->container.erase(this->container.begin() + at);
		--at;
	}

	// we have to cancel one --at in order to return
	// the element after the group we deleted.
	if (at != this->container.size()) {
		++at;
	}

	return at;
}

} // namespace openage::curve
