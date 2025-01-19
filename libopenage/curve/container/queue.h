// Copyright 2017-2025 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <iostream>
#include <iterator>
#include <list>
#include <memory>
#include <string>

#include "error/error.h"

#include "curve/container/element_wrapper.h"
#include "curve/container/iterator.h"
#include "curve/container/queue_filter_iterator.h"
#include "event/evententity.h"
#include "time/time.h"
#include "util/fixed_point.h"


namespace openage {
namespace event {
class EventLoop;
}

namespace curve {

/**
 * A container that manages events on a timeline. Every event has exactly one
 * time it will happen.
 * This container can be used to store interactions
 */
template <class T>
class Queue : public event::EventEntity {
public:
	/**
	 * The underlaying container type.
	 */
	using container_t = typename std::vector<element_wrapper<T>>;

	/**
	 * The index type to access elements in the container
	 */
	using elem_ptr = typename container_t::size_type;

	/**
	 * The iterator type to access elements in the container
	 */
	using const_iterator = typename container_t::const_iterator;
	using iterator = typename container_t::iterator;

	Queue(const std::shared_ptr<event::EventLoop> &loop,
	      size_t id,
	      const std::string &idstr = "") :
		EventEntity{loop},
		_id{id},
		_idstr{idstr},
		last_change{time::TIME_ZERO},
		front_start{0} {}

	// prevent accidental copy of queue
	Queue(const Queue &) = delete;

	// Reading Access

	/**
	 * Get the first element inserted at t <= time.
	 *
	 * Ignores dead elements.
	 *
	 * @param time The time to get the element at.
	 *
	 * @return Queue element.
	 */
	const T &front(const time::time_t &time) const;

	/**
	 * Check if the queue is empty at the given time (no elements alive
	 * before t <= time).
	 *
	 * Ignores dead elements.
	 *
	 * @param time The time to check at.
	 *
	 * @return true if the queue is empty, false otherwise.
	 */
	bool empty(const time::time_t &time) const;

	// Modifying access

	/**
	 * Get the first element inserted at t <= time and erase it from the
	 * queue.
	 *
	 * Ignores dead elements.
	 *
	 * @param time The time to get the element at.
	 * @param value Queue element.
	 */
	const T &pop_front(const time::time_t &time);

	/**
	 * Get an iterator to the first element inserted at t >= time.
	 *
	 * Does not ignore dead elements.
	 *
	 * @param time The time to get the element at (default: \p time::TIME_MIN ).
	 *
	 * @return Iterator to the first element.
	 */
	QueueFilterIterator<T, Queue<T>> begin(const time::time_t &time = time::TIME_MIN) const;

	/**
	 * Get an iterator to the last element in the queue at the given time.
	 *
	 * Does not ignore dead elements.
	 *
	 * @param t The time to get the element at (default: \p time::TIME_MAX ).
	 *
	 * @return Iterator to the last element.
	 */
	QueueFilterIterator<T, Queue<T>> end(const time::time_t &time = time::TIME_MAX) const;

	/**
	 * Get an iterator to elements that are in the queue between two time frames.
	 *
	 * Does not ignore dead elements.
	 *
	 * @param begin Start time (default: \p time::TIME_MIN ).
	 * @param end End time (default: \p time::TIME_MAX ).
	 *
	 * @return Iterator to the first element in the time frame.
	 */
	QueueFilterIterator<T, Queue<T>> between(
		const time::time_t &begin = time::TIME_MIN,
		const time::time_t &end = time::TIME_MAX) const;

	/**
	 * Erase an element from the queue.
	 *
	 * Does not ignore dead elements.
	 *
	 * @param it The iterator to the element to erase.
	 */
	void erase(const CurveIterator<T, Queue<T>> &it);

	/**
	 * Insert a new element into the queue.
	 *
	 * @param time The time to insert at.
	 * @param e The element to insert.
	 *
	 * @return Iterator to the inserted element.
	 */
	QueueFilterIterator<T, Queue<T>> insert(const time::time_t &time, const T &e);

	/**
	 * Erase all elements at t <= time.
	 *
	 * @param time The time to clear at.
	 */
	void clear(const time::time_t &time);

	/**
	 * Print the queue to stdout.
	 */
	void dump() {
		for (auto i : container) {
			std::cout << i.value << " at " << i.alive() << std::endl;
		}
	}

	/**
	 * Get the identifier of this curve.
	 *
	 * @return Identifier.
	 */
	size_t id() const override {
		return this->_id;
	}

	/**
	 * Get the human-readable identifier of this curve.
	 *
	 * @return Human-readable identifier.
	 */
	std::string idstr() const override {
		if (this->_idstr.size() == 0) {
			return std::to_string(this->id());
		}
		return this->_idstr;
	}

private:
	/**
	 * Kill an element from the queue at the given time.
	 *
	 * The element is set to dead at the given time and is not accessible
	 * for pops with t > time.
	 *
	 * @param time The time to kill at.
	 * @param at The index of the element to kill.
	 */
	void kill(const time::time_t &time, elem_ptr at);

	/**
	 * Get the first alive element inserted at t <= time.
	 *
	 * @param time The time to get the element at.
	 *
	 * @return Index of the first alive element or end() if no such element exists.
	 */
	elem_ptr first_alive(const time::time_t &time) const;

	/**
	 * Identifier for the container
	 */
	const size_t _id;

	/**
	 * Human-readable identifier for the container
	 */
	const std::string _idstr;

	/**
	 * The container that stores the queue elements.
	 */
	container_t container;

	/**
	 * Simulation time of the last modifying change to the queue.
	 */
	time::time_t last_change;

	/**
	 * Caches the search start position for the next front() call.
	 *
	 * All positions before the index are guaranteed to be dead at t >= last_change.
	 */
	elem_ptr front_start;
};


template <class T>
typename Queue<T>::elem_ptr Queue<T>::first_alive(const time::time_t &time) const {
	elem_ptr hint = 0;

	// check if the access is later than the last change
	if (this->last_change <= time) {
		// start searching from the last front position
		hint = this->front_start;
	}
	// else search from the beginning

	// Iterate until we find an alive element
	while (hint != this->container.size()
	       and this->container.at(hint).alive() <= time) {
		if (this->container.at(hint).dead() > time) {
			return hint;
		}
		++hint;
	}

	return this->container.size();
}


template <typename T>
const T &Queue<T>::front(const time::time_t &time) const {
	elem_ptr at = this->first_alive(time);
	ENSURE(at < this->container.size(),
	       "Tried accessing front at " << time << " but index " << at << " is invalid. "
	                                   << "The queue may be empty."
	                                   << "(last_change: " << this->last_change
	                                   << ", front_start: " << this->front_start
	                                   << ", container size: " << this->container.size()
	                                   << ")");

	return this->container.at(at).value();
}


template <class T>
const T &Queue<T>::pop_front(const time::time_t &time) {
	elem_ptr at = this->first_alive(time);
	ENSURE(at < this->container.size(),
	       "Tried accessing front at " << time << " but index " << at << " is invalid. "
	                                   << "The queue may be empty."
	                                   << "(last_change: " << this->last_change
	                                   << ", front_start: " << this->front_start
	                                   << ", container size: " << this->container.size()
	                                   << ")");

	// kill the element at time t
	this->kill(time, at);

	// cache the search start position for the next front() call
	// for pop time t, there should be no more elements alive before t
	// so we can advance the front to the next element
	this->last_change = time;
	this->front_start = at + 1;

	this->changes(time);

	return this->container.at(at).value();
}


template <class T>
bool Queue<T>::empty(const time::time_t &time) const {
	if (this->container.empty()) {
		return true;
	}

	return this->first_alive(time) == this->container.size();
}


template <typename T>
QueueFilterIterator<T, Queue<T>> Queue<T>::begin(const time::time_t &t) const {
	for (auto it = this->container.begin(); it != this->container.end(); ++it) {
		if (it->alive() >= t) {
			return QueueFilterIterator<T, Queue<T>>(
				it,
				this,
				t,
				time::TIME_MAX);
		}
	}

	return this->end(t);
}


template <typename T>
QueueFilterIterator<T, Queue<T>> Queue<T>::end(const time::time_t &t) const {
	return QueueFilterIterator<T, Queue<T>>(
		container.end(),
		this,
		t,
		time::TIME_MAX);
}


template <typename T>
QueueFilterIterator<T, Queue<T>> Queue<T>::between(const time::time_t &begin,
                                                   const time::time_t &end) const {
	auto it = QueueFilterIterator<T, Queue<T>>(
		container.begin(),
		this,
		begin,
		end);
	if (not it.valid()) {
		++it;
	}
	return it;
}


template <typename T>
void Queue<T>::erase(const CurveIterator<T, Queue<T>> &it) {
	container.erase(it.get_base());
}


template <class T>
void Queue<T>::kill(const time::time_t &time,
                    elem_ptr at) {
	this->container[at].set_dead(time);
}


template <typename T>
QueueFilterIterator<T, Queue<T>> Queue<T>::insert(const time::time_t &time,
                                                  const T &e) {
	elem_ptr at = this->container.size();
	while (at > 0) {
		--at;
		if (this->container.at(at).alive() <= time) {
			++at;
			break;
		}
	}

	// Get the iterator to the insertion point
	iterator insertion_point = std::next(this->container.begin(), at);
	insertion_point = this->container.insert(insertion_point, element_wrapper<T>{time, e});

	// TODO: Inserting before any dead elements shoud reset their death time
	//       since by definition, they cannot be popped before the new element

	// cache the insertion time
	this->last_change = time;

	// if the new element is inserted before the current front element
	// cache it as the new front element
	if (at < this->front_start) {
		this->front_start = at;
	}

	auto ct = QueueFilterIterator<T, Queue<T>>(
		insertion_point,
		this,
		time,
		time::TIME_MAX);

	if (!ct.valid()) {
		++ct;
	}

	this->changes(time);

	return ct;
}


template <typename T>
void Queue<T>::clear(const time::time_t &time) {
	elem_ptr at = this->first_alive(time);

	// no elements alive at t <= time
	// so we don't have any changes
	if (at == this->container.size()) {
		return;
	}

	// erase all elements alive at t <= time
	while (this->container.at(at).alive() <= time
	       and at != this->container.size()) {
		if (this->container.at(at).dead() > time) {
			this->container[at].set_dead(time);
		}
		++at;
	}

	// cache the search start position for the next front() call
	this->last_change = time;
	this->front_start = at;

	this->changes(time);
}


} // namespace curve
} // namespace openage
