// Copyright 2017-2025 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <functional>
#include <iostream>
#include <list>

#include "curve/concept.h"
#include "curve/keyframe.h"
#include "time/time.h"
#include "util/fixed_point.h"


namespace openage::curve {

/**
 * A container storing time-value (keyframe) pairs ordered by time.
 *
 * This class has several management functions for modifying and accessing the
 * underlying storage. For getting a keyframe value, the exact timestamp does not
 * have to be known, it will always return the one closest, less or equal to the
 * requested one.
 **/
template <KeyframeValueLike T>
class KeyframeContainer {
public:
	/**
	 * Element of the container. Represents a single time-value pair.
	 */
	using keyframe_t = Keyframe<T>;

	/**
	 * Underlaying container type.
	 */
	using container_t = std::vector<keyframe_t>;

	/**
	 * Index type to access elements in the container
	 */
	using elem_ptr = typename container_t::size_type;

	/**
	 * Iterator type to access elements in the container.
	 */
	using iterator = typename container_t::const_iterator;

	/**
	 * Create a new keyframe container.
	 *
	 * Inserts a default element with value \p T() at \p time = -INF to ensure
	 * that accessing the container always returns an element.
	 *
	 * TODO: need the datamanger for change management
	 */
	KeyframeContainer();

	/**
	 * Create a new keyframe container.
	 *
	 * Inserts a default element \p defaultval at \p time = -INF to ensure
	 * that accessing the container always returns an element.
	 *
	 * @param defaultval Value of default element at -INF.
	 *
	 * TODO: need the datamanger for change management
	 */
	KeyframeContainer(const T &defaultval);

	/**
	 * Get the number of elements in this container.
	 */
	size_t size() const;

	/**
	 * Get the value of the keyframe at the given index.
	 *
	 * @param idx Index of the keyframe to get.
	 *
	 * @return The keyframe at the given index.
	 */
	const keyframe_t &get(const elem_ptr &idx) const {
		return this->container.at(idx);
	}

	/**
	 * Get the last element in the container which is at or before the given time.
	 * (i.e. elem->time <= time). Include a hint where to start the search.
	 *
	 * @param time Request time.
	 * @param hint Index of the approximate element location.
	 *
	 * @return Last element with time <= time.
	 */
	elem_ptr last(const time::time_t &time,
	              const elem_ptr &hint) const;

	/**
	 * Get the last element in the container which is at or before the given time.
	 * (i.e. elem->time <= time).
	 *
	 * The usage of this method is discouraged - except if there is absolutely
	 * no chance for you to have a hint (or the container is known to be nearly
	 * empty).
	 *
	 * @param time Request time.
	 *
	 * @return Last element with time <= time.
	 */
	elem_ptr last(const time::time_t &time) const {
		return this->last(time, this->container.size());
	}

	/**
	 * Get the last element in the container which is before the given time.
	 * (i.e. elem->time < time). Include a hint where to start the search.
	 *
	 * @param time Request time.
	 * @param hint Index of the approximate element location.
	 *
	 * @return Last element with time < time.
	 */
	elem_ptr last_before(const time::time_t &time,
	                     const elem_ptr &hint) const;

	/**
	 * Get the last element in the container which is before the given time.
	 *
	 * The usage of this method is discouraged - except if there is absolutely
	 * no chance for you to have a hint (or the container is known to be nearly
	 * empty).
	 *
	 * @param time Request time.
	 *
	 * @return Last element with time < time.
	 */
	elem_ptr last_before(const time::time_t &time) const {
		return this->last_before(time, this->container.size());
	}

	/**
	 * Insert a new element. The search for the insertion point is
	 * started from the end of the data.
	 *
	 * The use of this function is discouraged, use it only, if you really
	 * do not have the possibility to get a hint.
	 *
	 * If there is already a keyframe with identical time in the container, this will
	 * insert the new keyframe before the old one.
	 *
	 * @param keyframe Keyframe to insert.
	 *
	 * @return Location (index) of the inserted element.
	 */
	elem_ptr insert_before(const keyframe_t &keyframe) {
		return this->insert_before(keyframe, this->container.size());
	}

	/**
	 * Insert a new element.
	 *
	 * The hint shall give an approximate location, where
	 * the inserter will start to look for an insertion point. If a good hint is
	 * given, the runtime of this function will not be affected by the current
	 * history size.
	 *
	 * If there is a keyframe with identical time, this will
	 * insert the new keyframe before the old one.
	 *
	 * @param keyframe Keyframe to insert.
	 * @param hint Index of the approximate insertion location.
	 *
	 * @return The location (index) of the inserted element.
	 */
	elem_ptr insert_before(const keyframe_t &keyframe,
	                       const elem_ptr &hint);

	/**
	 * Create and insert a new element. The search for the insertion point is
	 * started from the end of the data.
	 *
	 * The use of this function is discouraged, use it only, if you really
	 * do not have the possibility to get a hint.
	 *
	 * If there is a keyframe with identical time in the container, this will
	 * insert the new keyframe before the old one.
	 *
	 * @param time Time of the new keyframe.
	 * @param value Value of the new keyframe.
	 *
	 * @return Location (index) of the inserted element.
	 */
	elem_ptr insert_before(const time::time_t &time,
	                       const T &value) {
		return this->insert_before(keyframe_t(time, value),
		                           this->container.size());
	}

	/**
	 * Create and insert a new element.
	 *
	 * The hint shall give an approximate location, where
	 * the inserter will start to look for an insertion point. If a good hint is
	 * given, the runtime of this function will not be affected by the current
	 * history size.
	 *
	 * If there is a value with identical time, this will insert the new value
	 * before the old one.
	 *
	 * @param time Time of the new keyframe.
	 * @param value Value of the new keyframe.
	 * @param hint Index of the approximate insertion location.
	 *
	 * @return Location (index) of the inserted element.
	 */
	elem_ptr insert_before(const time::time_t &time,
	                       const T &value,
	                       const elem_ptr &hint) {
		return this->insert_before(keyframe_t(time, value), hint);
	}

	/**
	 * Insert a new element, overwriting elements that have a
	 * time conflict. The hint gives an approximate insertion location to minimize runtime
	 * on big-history curves.
	 *
	 * `overwrite_all` == true -> overwrite all same-time elements.
	 * `overwrite_all` == false -> overwrite the last of the time-conflict elements.
	 *
	 * @param keyframe Keyframe to insert.
	 * @param hint Index of the approximate insertion location.
	 * @param overwrite_all If true, overwrite all elements with the same time.
	 *                      If false, overwrite only the last element with the same time.
	 *
	 * @return Location (index) of the inserted element.
	 */
	elem_ptr insert_overwrite(const keyframe_t &keyframe,
	                          const elem_ptr &hint,
	                          bool overwrite_all = false);

	/**
	 * Create and insert a new value at given time which will overwrite the last of the
	 * elements with the same time. This function will start to search the time
	 * from the end of the data.
	 *
	 * The use of this function is discouraged, use it only, if you really
	 * do not have the possibility to get a hint.
	 *
	 * @param time Time of the new keyframe.
	 * @param value Value of the new keyframe.
	 *
	 * @return Location (index) of the inserted element.
	 */
	elem_ptr insert_overwrite(const time::time_t &time, const T &value) {
		return this->insert_overwrite(keyframe_t(time, value),
		                              this->container.size());
	}

	/**
	 * Insert a new value at given time, which overwrites element(s) with
	 * identical time. Provide a insertion hint to abbreviate the search for the
	 * insertion point.
	 *
	 * `overwrite_all` == true -> overwrite all same-time elements.
	 * `overwrite_all` == false -> overwrite the last of the time-conflict elements.
	 *
	 * @param time Time of the new keyframe.
	 * @param value Value of the new keyframe.
	 * @param hint Index of the approximate insertion location.
	 * @param overwrite_all If true, overwrite all elements with the same time.
	 *                      If false, overwrite only the last element with the same time.
	 *
	 * @return Location (index) of the inserted element.
	 */
	elem_ptr insert_overwrite(const time::time_t &time,
	                          const T &value,
	                          const elem_ptr &hint,
	                          bool overwrite_all = false) {
		return this->insert_overwrite(keyframe_t(time, value), hint, overwrite_all);
	}

	/**
	 * Insert a new element, after a previous element when there's a time
	 * conflict. The hint gives an approximate insertion location to minimize runtime on
	 * big-history curves.
	 *
	 * @param keyframe Keyframe to insert.
	 * @param hint Index of the approximate insertion location.
	 *
	 * @return Location (index) of the inserted element.
	 */
	elem_ptr insert_after(const keyframe_t &keyframe,
	                      const elem_ptr &hint);

	/**
	 * Create and insert a new value at given time which will be prepended to the block of
	 * elements that have the same time. This function will start to search the
	 * time from the end of the data.
	 *
	 * The use of this function is discouraged, use it only, if you really
	 * do not have the possibility to get a hint.
	 *
	 * @param time Time of the new keyframe.
	 * @param value Value of the new keyframe.
	 *
	 * @return Location (index) of the inserted element.
	 */
	elem_ptr insert_after(const time::time_t &time,
	                      const T &value) {
		return this->insert_after(keyframe_t(time, value),
		                          this->container.size());
	}

	/**
	 * Create and insert a new element, which is added after a previous element with
	 * identical time. Provide a insertion hint to abbreviate the search for the insertion point.
	 *
	 * @param time Time of the new keyframe.
	 * @param value Value of the new keyframe.
	 * @param hint Index of the approximate insertion location.
	 *
	 * @return Location (index) of the inserted element.
	 */
	elem_ptr insert_after(const time::time_t &time,
	                      const T &value,
	                      const elem_ptr &hint) {
		return this->insert_after(keyframe_t(time, value), hint);
	}

	/**
	 * Erase all elements after the given element.
     *
     * @param last_valid Location of the last element to keep.
     *
     * @return Location (index) of the last element that was kept.
	 */
	elem_ptr erase_after(elem_ptr last_valid);

	/**
	 * Erase a single element from the container.
     *
     * @param it Location of the element to erase.
     *
     * @return Location (index) of the next element after the erased one.
	 */
	elem_ptr erase(elem_ptr it);

	/**
	 * Erase all elements with given time. Starts the search at the end of the container.
	 *
     * @param time Time of the elements to erase.
     *
     * @return Location (index) of the next element after the erased one.
	 */
	elem_ptr erase(const time::time_t &time) {
		return this->erase(time, this->container.size());
	}

	/**
	 * Erase all element with given time. Include a hint where to start the search.
	 *
	 * @param time Time of the elements to erase.
     * @param hint Index of the approximate element location.
     *
     * @return Location (index) of the next element after the erased one.
	 */
	elem_ptr erase(const time::time_t &time,
	               const elem_ptr &hint) {
		return this->erase_group(time, this->last(time, hint));
	}

	/**
	 * Get an iterator to the first keyframe in the container.
	 */
	iterator begin() const {
		return this->container.begin();
	}

	/**
	 * Get an iterator to the end of the container.
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
	template <KeyframeValueLike O>
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


template <KeyframeValueLike T>
KeyframeContainer<T>::KeyframeContainer() {
	this->container.push_back(keyframe_t(time::TIME_MIN, T()));
}


template <KeyframeValueLike T>
KeyframeContainer<T>::KeyframeContainer(const T &defaultval) {
	this->container.push_back(keyframe_t(time::TIME_MIN, defaultval));
}


template <KeyframeValueLike T>
size_t KeyframeContainer<T>::size() const {
	return this->container.size();
}


template <KeyframeValueLike T>
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


template <KeyframeValueLike T>
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


template <KeyframeValueLike T>
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


template <KeyframeValueLike T>
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


template <KeyframeValueLike T>
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


template <KeyframeValueLike T>
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


template <KeyframeValueLike T>
typename KeyframeContainer<T>::elem_ptr
KeyframeContainer<T>::erase(KeyframeContainer<T>::elem_ptr e) {
	this->container.erase(this->begin() + e);
	return e;
}


template <KeyframeValueLike T>
typename KeyframeContainer<T>::elem_ptr
KeyframeContainer<T>::sync(const KeyframeContainer<T> &other,
                           const time::time_t &start) {
	// Delete elements from this container after start time
	elem_ptr at = this->last_before(start, this->container.size());
	at = this->erase_after(at);

	// Find the last element before the start time in the other container
	elem_ptr at_other = other.last_before(start, other.size());
	++at_other; // move one element forward so that at_other.time() >= start

	// Copy all elements from other with time >= start
	this->container.insert(this->container.end(),
	                       other.container.begin() + at_other,
	                       other.container.end());

	return this->container.size();
}


template <KeyframeValueLike T>
template <KeyframeValueLike O>
typename KeyframeContainer<T>::elem_ptr
KeyframeContainer<T>::sync(const KeyframeContainer<O> &other,
                           const std::function<T(const O &)> &converter,
                           const time::time_t &start) {
	// Delete elements after start time
	elem_ptr at = this->last_before(start, this->container.size());
	at = this->erase_after(at);

	// Find the last element before the start time in the other container
	elem_ptr at_other = other.last_before(start, other.size());
	++at_other; // move one element forward so that at_other.time() >= start

	// Copy all elements from other with time >= start
	for (size_t i = at_other; i < other.size(); i++) {
		auto &elem = other.get(i);
		this->container.emplace_back(elem.time(), converter(elem.val()));
	}

	return this->container.size();
}


template <KeyframeValueLike T>
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
