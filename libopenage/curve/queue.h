// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "curve.h"
#include "queue_filter_iterator.h"

#include <deque>
#include <iostream>

namespace openage {
namespace curve {

template <class _T>
class Queue {
	struct queue_wrapper {
		curve_time_t _time;
		_T value;

		queue_wrapper(const curve_time_t &time, const _T &value) :
			_time{time},
			value{value} {}

		curve_time_t time() {
			return _time;
		}
	};
public:
	typedef typename std::deque<queue_wrapper> container_t;
	typedef typename container_t::iterator iterator;
	// Reading Access
	const _T &front(const curve_time_t &) const;

	// Modifying access
	QueueFilterIterator<_T, Queue<_T>> begin(
		const curve_time_t &t = -std::numeric_limits<curve_time_t>::infinity());

	QueueFilterIterator<_T, Queue<_T>> end(
		const curve_time_t &t = std::numeric_limits<curve_time_t>::infinity());

	QueueFilterIterator<_T, Queue<_T>> between(
		const curve_time_t &begin = std::numeric_limits<curve_time_t>::infinity(),
		const curve_time_t &end = std::numeric_limits<curve_time_t>::infinity());

	QueueFilterIterator<_T, Queue<_T>> erase(
			const CurveIterator<_T, Queue<_T>> &);

	QueueFilterIterator<_T, Queue<_T>> insert(
			const curve_time_t &, const _T &e);

	void __attribute__((noinline)) dump() {
		for (auto i : container) {
			std::cout << i.value << " at " << i.time() << std::endl;
		}
	}

private:
	container_t container;
};


template <typename _T>
const _T &Queue<_T>::front(const curve_time_t &) const {
	return container.front();
}


template <typename _T>
QueueFilterIterator<_T, Queue<_T>> Queue<_T>::begin(const curve_time_t &t)
{
	for (auto it = this->container.begin(); it != this->container.end(); ++it) {
		if (it->time() >= t) {
			return QueueFilterIterator<_T, Queue<_T>>(
				it,
				container.end(),
				t,
				std::numeric_limits<curve_time_t>::infinity());
		}
	}

	return this->end(t);
}


template <typename _T>
QueueFilterIterator<_T, Queue<_T>> Queue<_T>::end(const curve_time_t &t)
{
	return QueueFilterIterator<_T, Queue<_T>>(container.end(),
		container.end(),
		t,
		std::numeric_limits<curve_time_t>::infinity());
}


template <typename _T>
QueueFilterIterator<_T, Queue<_T>> Queue<_T>::between(
	const curve_time_t &begin,
	const curve_time_t &end)
{
	auto it = QueueFilterIterator<_T, Queue<_T>>(
		container.begin(),
		container.end(),
		begin,
		end);
	if (!it.valid()) {
		++it;
	}
	return it;
}


template <typename _T>
QueueFilterIterator<_T, Queue<_T>> Queue<_T>::erase(const CurveIterator<_T, Queue<_T>> &t)
{
	auto it = container.erase(t.base);
	auto ct = QueueFilterIterator<_T, Queue<_T>>(
		it,
		container.end(),
		t.from,
		t.to);

	if (!ct.valid(t.from)) {
		++ct;
	}
	return ct;
}


template <typename _T>
QueueFilterIterator<_T, Queue<_T>> Queue<_T>::insert(
		const curve_time_t &time,
        const _T &e) {
	iterator insertion_point = this->container.end();
	for (auto it = this->container.begin(); it != this->container.end(); ++it) {
		if (time < it->time()) {
			insertion_point = this->container
							  .insert(it, queue_wrapper(time, e));
			break;
		}
	}
	if (insertion_point == this->container.end()) {
		insertion_point = this->container.insert(this->container.end(),
												 queue_wrapper(time, e));
	}

	auto ct = QueueFilterIterator<_T, Queue<_T>>(
		insertion_point,
		container.end(),
		time, std::numeric_limits<curve_time_t>::infinity());

	if (!ct.valid()) {
		++ct;
	}
	return ct;
}

}} // openage::curve
