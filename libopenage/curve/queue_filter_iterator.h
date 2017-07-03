// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "curve.h"
#include "iterator.h"

#include <iterator>
#include <limits>

namespace openage {
namespace curve {

/**
 * A filtering operator to iterate over all elements of a queue whose elements
 * exist at exactly one point of time, the range where to iterate is given at
 * construction.
 *
 * It depends on val_t as its value type, container_t is the container
 * to operate on and the function valid_f, that checks if an element is alive.
 */
template <class val_t,
          class container_t>
class QueueFilterIterator :
		public CurveIterator<val_t, container_t, typename container_t::const_iterator>
{
public:
	typedef typename container_t::const_iterator const_iterator;

	/**
	 * Construct the iterator from its boundary conditions: time and container
	 */
	QueueFilterIterator(const const_iterator &base,
	                    const container_t *base_container,
	                    const curve_time_t &from,
	                    const curve_time_t &to) :
		CurveIterator<val_t, container_t>(base, base_container, from, to) {}

	virtual bool valid() const override {
		if (this->container->end().base() != this->base()) {
			return this->base()->time() >= this->from && this->base()->time() < this->to;
		}
		return false;
	}

	const val_t &value() const override {
		const auto &a = (*this->base());
		return a.value;
	}
};

}} // openage::curve
