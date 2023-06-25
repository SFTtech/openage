// Copyright 2017-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <deque>
#include <iostream>

#include "curve/curve.h"
#include "curve/queue_filter_iterator.h"
#include "event/evententity.h"


namespace openage::curve {

/**
 * A container that manages events on a timeline. Every event has exactly one
 * time it will happen.
 * This container can be used to store interactions
 */
template <class T>
class Queue : public event::EventEntity {
	struct queue_wrapper {
		time_t _time;
		T value;

		queue_wrapper(const time_t &time, const T &value) :
			_time{time},
			value{value} {}

		time_t time() const {
			return _time;
		}
	};

public:
	using container_t = typename std::deque<queue_wrapper>;
	using const_iterator = typename container_t::const_iterator;
	using iterator = typename container_t::const_iterator;

	Queue(const std::shared_ptr<event::EventLoop> &loop,
	      size_t id,
	      const std::string &idstr = "") :
		EventEntity{loop},
		_id{id},
		_idstr{idstr} {}

	// Reading Access

	/**
	 * Get the first element in the queue at the given time.
	 *
	 * @param time The time to get the element at.
	 * @return Queue element.
	 */
	const T
		&
		front(const time_t &time) const;

	// Modifying access

	/**
	 * Get an iterator to the first/front element in the queue at the given time.
	 *
	 * @param t The time to get the element at.
	 * @return Iterator to the first element.
	 */
	QueueFilterIterator<T, Queue<T>> begin(
		const time_t &t = -std::numeric_limits<time_t>::max()) const;

	/**
	 * Get an iterator to the last element in the queue at the given time.
	 *
	 * @param t The time to get the element at.
	 * @return Iterator to the last element.
	 */
	QueueFilterIterator<T, Queue<T>> end(
		const time_t &t = std::numeric_limits<time_t>::max()) const;

	/**
	 * Get an iterator to elements that are in the queue between two time frames.
	 *
	 * @param begin Start time.
	 * @param end End time.
	 * @return Iterator to the first element in the time frame.
	 */
	QueueFilterIterator<T, Queue<T>> between(
		const time_t &begin = std::numeric_limits<time_t>::max(),
		const time_t &end = std::numeric_limits<time_t>::max()) const;

	/**
	 * Erase an element from the queue.
	 *
	 * @param it The iterator to the element to erase.
	 */
	void erase(const CurveIterator<T, Queue<T>> &it);

	/**
	 * Insert a new element into the queue.
	 *
	 * @param time The time to insert at.
	 * @param e The element to insert.
	 * @return Iterator to the inserted element.
	 */
	QueueFilterIterator<T, Queue<T>> insert(const time_t &, const T &e);

	/**
	 * Erase all elements that are at or after the given time.
	 *
	 * @param time The time to clear at.
	 */
	void clear(const time_t &);

	/**
	 * Print the queue to stdout.
	 */
	void dump() {
		for (auto i : container) {
			std::cout << i.value << " at " << i.time() << std::endl;
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
	 * The container that stores the queue elements.
	 */
	container_t container;

	/**
	 * Identifier for the container
	 */
	const size_t _id;

	/**
	 * Human-readable identifier for the container
	 */
	const std::string _idstr;
};


template <typename T>
const T &Queue<T>::front(const time_t &t) const {
	return this->begin(t).value();
}


template <typename T>
QueueFilterIterator<T, Queue<T>> Queue<T>::begin(const time_t &t) const {
	for (auto it = this->container.begin(); it != this->container.end(); ++it) {
		if (it->time() >= t) {
			return QueueFilterIterator<T, Queue<T>>(
				it,
				this,
				t,
				std::numeric_limits<time_t>::max());
		}
	}

	return this->end(t);
}


template <typename T>
QueueFilterIterator<T, Queue<T>> Queue<T>::end(const time_t &t) const {
	return QueueFilterIterator<T, Queue<T>>(
		container.end(),
		this,
		t,
		std::numeric_limits<time_t>::max());
}


template <typename T>
QueueFilterIterator<T, Queue<T>> Queue<T>::between(
	const time_t &begin,
	const time_t &end) const {
	auto it = QueueFilterIterator<T, Queue<T>>(
		container.begin(),
		this,
		begin,
		end);
	if (!container.empty() && !it.valid()) {
		++it;
	}
	return it;
}


template <typename T>
void Queue<T>::erase(const CurveIterator<T, Queue<T>> &it) {
	container.erase(it.base());
	return;
}


template <typename T>
QueueFilterIterator<T, Queue<T>> Queue<T>::insert(
	const time_t &time,
	const T &e) {
	const_iterator insertion_point = this->container.end();
	for (auto it = this->container.begin(); it != this->container.end(); ++it) {
		if (time < it->time()) {
			insertion_point = this->container.insert(it, queue_wrapper(time, e));
			break;
		}
	}
	if (insertion_point == this->container.end()) {
		insertion_point = this->container.insert(this->container.end(),
		                                         queue_wrapper(time, e));
	}

	auto ct = QueueFilterIterator<T, Queue<T>>(
		insertion_point,
		this,
		time,
		std::numeric_limits<time_t>::max());

	if (!ct.valid()) {
		++ct;
	}

	this->changes(time);

	return ct;
}


template <typename T>
void Queue<T>::clear(const time_t &time) {
	for (auto it = this->container.begin();
	     it != this->container.end() and it->time() < time;
	     it = this->container.erase(it)) {
	}

	this->changes(time);
}


} // namespace openage::curve
