// Copyright 2017-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <deque>
#include <iostream>
#include <memory>
#include <string>

#include "curve/iterator.h"
#include "curve/queue_filter_iterator.h"
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
	struct queue_wrapper {
		// Insertion time of the element
		time::time_t _alive;
		// Erase time of the element
		time::time_t _dead;
		// Element value
		T value;

		queue_wrapper(const time::time_t &time, const T &value) :
			_alive{time},
			_dead{time::TIME_MAX},
			value{value} {}

		time::time_t alive() const {
			return _alive;
		}

		time::time_t dead() const {
			return _dead;
		}

		void set_dead(const time::time_t &time) {
			_dead = time;
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
		_idstr{idstr},
		last_pop{time::TIME_ZERO} {}

	// prevent accidental copy of queue
	Queue(const Queue &) = delete;

	// Reading Access

	/**
	 * Get the first element in the queue at the given time.
	 *
	 * @param time The time to get the element at.
	 * @return Queue element.
	 */
	const T &front(const time::time_t &time) const;

	/**
	 * Get the first element in the queue at the given time and remove it from
	 * the queue.
	 *
	 * @param time The time to get the element at.
	 * @param value Queue element.
	 */
	const T pop_front(const time::time_t &time);

	/**
	 * Check if the queue is empty at a given time.
	 *
	 * @return true if the queue is empty, false otherwise.
	 */
	bool empty(const time::time_t &time) const;

	// Modifying access

	/**
	 * Get an iterator to the first/front element in the queue at the given time.
	 *
	 * @param t The time to get the element at.
	 * @return Iterator to the first element.
	 */
	QueueFilterIterator<T, Queue<T>> begin(
		const time::time_t &t = -time::TIME_MAX) const;

	/**
	 * Get an iterator to the last element in the queue at the given time.
	 *
	 * @param t The time to get the element at.
	 * @return Iterator to the last element.
	 */
	QueueFilterIterator<T, Queue<T>> end(
		const time::time_t &t = time::TIME_MAX) const;

	/**
	 * Get an iterator to elements that are in the queue between two time frames.
	 *
	 * @param begin Start time.
	 * @param end End time.
	 * @return Iterator to the first element in the time frame.
	 */
	QueueFilterIterator<T, Queue<T>> between(
		const time::time_t &begin = time::TIME_MAX,
		const time::time_t &end = time::TIME_MAX) const;

	/**
	 * Erase an element from the queue.
	 *
	 * @param it The iterator to the element to erase.
	 */
	void erase(const CurveIterator<T, Queue<T>> &it);

	/**
	 * Erase an element from the queue at a specific time.
	 *
     * @param time The time to erase at.
	 * @param it The iterator to the element to erase.
	 */
	void erase(const time::time_t &time, const CurveIterator<T, Queue<T>> &it);

	/**
	 * Insert a new element into the queue.
	 *
	 * @param time The time to insert at.
	 * @param e The element to insert.
	 * @return Iterator to the inserted element.
	 */
	QueueFilterIterator<T, Queue<T>> insert(const time::time_t &time, const T &e);

	/**
	 * Erase all elements that are at or after the given time.
	 *
	 * @param time The time to clear at.
	 */
	void clear(const time::time_t &);

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
	 * The time of the last access to the queue.
	 */
	time::time_t last_pop;
};


template <typename T>
const T &Queue<T>::front(const time::time_t &time) const {
	if (this->empty(time)) [[unlikely]] {
		throw Error{MSG(err) << "Tried accessing front at "
		                     << time << " but queue is empty."};
	}

	// search for the last element before the given time
	auto it = this->container.end();
	--it;
	while (it->alive() > time and it != this->container.begin()) {
		--it;
	}

	return it->value;
}

template <class T>
const T Queue<T>::pop_front(const time::time_t &time) {
	if (this->empty(time)) [[unlikely]] {
		throw Error{MSG(err) << "Tried accessing front at "
		                     << time << " but queue is empty."};
	}

	// search for the last element before the given time
	auto it = this->container.end();
	--it;
	while (it->alive() > time and it != this->container.begin()) {
		--it;
	}

	// get the last element inserted before the given time
	auto val = std::move(it->value);

	// get the time span between current time and the next element
	auto to = (++it)->alive();
	--it;
	auto from = time;

	// erase the element
	// TODO: We should be able to reinsert elements
	auto filter_iterator = QueueFilterIterator<T, Queue<T>>(it, this, to, from);
	this->erase(filter_iterator);

	this->last_pop = time;

	return val;
}

template <class T>
bool Queue<T>::empty(const time::time_t &time) const {
	if (this->container.empty()) {
		return true;
	}

	// search for the first element that is after the given time
	auto begin = this->begin(time).get_base();

	return begin == this->container.begin() and begin->alive() > time;
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
inline void Queue<T>::erase(const time::time_t &time,
                            const CurveIterator<T, Queue<T>> &it) {
	it.get_base()->set_dead(time);
}


template <typename T>
QueueFilterIterator<T, Queue<T>> Queue<T>::insert(const time::time_t &time,
                                                  const T &e) {
	const_iterator insertion_point = this->container.end();
	for (auto it = this->container.begin(); it != this->container.end(); ++it) {
		if (time < it->alive()) {
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
		time::TIME_MAX);

	if (!ct.valid()) {
		++ct;
	}

	this->changes(time);

	return ct;
}


template <typename T>
void Queue<T>::clear(const time::time_t &time) {
	for (auto it = this->container.begin();
	     it != this->container.end() and it->alive() < time;
	     it = this->container.erase(it)) {
	}

	this->changes(time);
}


} // namespace curve
} // namespace openage
