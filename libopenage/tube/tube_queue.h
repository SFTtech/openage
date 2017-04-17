// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once


#include "tube.h"
#include "queue_filter_iterator.h"

#include <deque>
#include <iostream>

namespace openage {
namespace tube {

template <class _T>
class Queue {
	struct queue_wrapper {
		tube_time_t _time;
		_T value;

		queue_wrapper(const tube_time_t &time, const _T &value) :
			_time{time},
			value{value} {}

		tube_time_t time() {
			return _time;
		}
	};
public:
	typedef typename std::deque<queue_wrapper> container_t;
	typedef typename container_t::iterator iterator;
	// Reading Access
	const _T &front(const tube_time_t &) const {
		return container.front();
	}

	// Modifying access
	TubeQueueFilterIterator<_T, Queue<_T>> begin(
		const tube_time_t &t = -std::numeric_limits<tube_time_t>::infinity())
	{
		for (auto it = this->container.begin(); it != this->container.end(); ++it) {
			if (it->time() >= t) {
				return TubeQueueFilterIterator<_T, Queue<_T>>(
					it,
					container.end(),
					t,
					std::numeric_limits<tube_time_t>::infinity());
			}
		}

		return this->end(t);
	}

	TubeQueueFilterIterator<_T, Queue<_T>> end(
		const tube_time_t &t = std::numeric_limits<tube_time_t>::infinity())
	{
		return TubeQueueFilterIterator<_T, Queue<_T>>(container.end(),
		    container.end(),
		    t,
		    std::numeric_limits<tube_time_t>::infinity());
	}

	TubeQueueFilterIterator<_T, Queue<_T>> between(
		const tube_time_t &begin = std::numeric_limits<tube_time_t>::infinity(),
		const tube_time_t &end = std::numeric_limits<tube_time_t>::infinity())
	{
		auto it = TubeQueueFilterIterator<_T, Queue<_T>>(
		    container.begin(),
		    container.end(),
		    begin,
		    end);
		if (!it.valid()) {
			++it;
		}
		return it;
	}

	TubeQueueFilterIterator<_T, Queue<_T>> erase(const TubeQueueFilterIterator<_T, Queue<_T>> &t) {
		auto it = container.erase(t.base);
		auto ct = TubeQueueFilterIterator<_T, Queue<_T>>(
		    it,
		    container.end(),
		    t.from,
		    t.to);

		if (!ct.valid(t.from)) {
			++ct;
		}
		return ct;
	}

	TubeQueueFilterIterator<_T, Queue<_T>> insert(const tube_time_t & time, const _T &e) {
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

		auto ct = TubeQueueFilterIterator<_T, Queue<_T>>(
			insertion_point,
			container.end(),
			time, std::numeric_limits<tube_time_t>::infinity());

		if (!ct.valid()) {
			++ct;
		}
		return ct;
	}

	void __attribute__((noinline)) dump() {
		for (auto i : container) {
			std::cout << i.value << " at " << i.time() << std::endl;
		}
	}

private:
	container_t container;
};

}} // openage::tube
