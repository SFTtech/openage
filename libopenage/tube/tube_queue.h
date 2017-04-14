// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once


#include "tube.h"
// #include "tube_filter_iterator.h"

#include <deque>

namespace openage {
namespace tube {

template <class _T>
class Queue {
public:
	// Reading Access
	const _T &front(const tube_time_t &) const;
// TODO define the TubeQueueIterator
//	tube_filter_iterator<_T, Queue> begin(const tube_time_t &);
//	tube_filter_iterator<_T, Queue> end(const tube_time_t &);

	// Modifying access
	void pop(const tube_time_t &);

private:
	std::deque<_T> container;
};

}} // openage::tube
