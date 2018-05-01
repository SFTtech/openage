// Copyright 2017-2018 the openage authors. See copying.md for legal info.

#pragma once

#include "curve.h"
#include "queue_filter_iterator.h"

#include <deque>
#include <iostream>

namespace openage::curve {

/**
 * A container that manages events on a timeline. Every event has exactly one
 * time it will happen.
 * This container can be used to store interactions
 */
template <class T>
class Queue {
	struct queue_wrapper {
		time_t _time;
		T value;

		queue_wrapper(const time_t &time, const T &value)
			:
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

	// Reading Access
	const T &front(const time_t &) const;

	// Modifying access
	QueueFilterIterator<T, Queue<T>> begin(
		const time_t &t = -std::numeric_limits<time_t>::max()) const;

	QueueFilterIterator<T, Queue<T>> end(
		const time_t &t = std::numeric_limits<time_t>::max()) const;

	QueueFilterIterator<T, Queue<T>> between(
		const time_t &begin = std::numeric_limits<time_t>::max(),
		const time_t &end = std::numeric_limits<time_t>::max()) const;

	void erase(const CurveIterator<T, Queue<T>> &);

	QueueFilterIterator<T, Queue<T>> insert(const time_t &, const T &e);

	void clear();

	void clean(const time_t &);

	void dump() {
		for (auto i : container) {
			std::cout << i.value << " at " << i.time() << std::endl;
		}
	}

private:
	container_t container;
};


template <typename T>
const T &Queue<T>::front(const time_t &) const {
	return container.front();
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
void Queue<T>::erase(const CurveIterator<T, Queue<T>> &t) {
	auto it = container.erase(t.base());
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
		time, std::numeric_limits<time_t>::max());

	if (!ct.valid()) {
		++ct;
	}
	return ct;
}


template <typename T>
void Queue<T>::clear() {
	this->container.clear();
}


template <typename T>
void Queue<T>::clean(const time_t &time) {
	for (auto it = this->container.begin();
	     it != this->container.end() and it->time() < time;
	     it = this->container.erase(it))
	{}
}


} // openage::curve
