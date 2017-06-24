// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <iterator>
#include <limits>

#include "curve.h"
#include "iterator.h"

namespace openage {
namespace curve {

/**
 * A filtering operator to iterate over all elements of a map whose elements
 * exist for a certain livespan. The range where to iterate is given at
 * construction.
 *
 * It depends on key_t and val_t as map-parameters, container_t is the container
 * to operate on and the function valid_f, that checks if an element is alive.
 */
template <class key_t,
          class val_t,
          class container_t>
class MapFilterIterator :
		public CurveIterator<val_t, container_t>
{
public:
	typedef typename container_t::const_iterator iterator_t;

	/**
	 * Construct the iterator from its boundary conditions: time and container
	 */
	MapFilterIterator(const iterator_t &base,
	                  const container_t *container,
	                  const curve_time_t &from,
	                  const curve_time_t &to) :
		CurveIterator<val_t, container_t>(base, container, from, to) {}

	MapFilterIterator(const MapFilterIterator &) = default;

	using CurveIterator<val_t, container_t>::operator=;

	virtual bool valid() const override {
		return this->base()->second.alive >= this->from
		&& this->base()->second.dead < this->to;
	}

	/**
	 * Get the value behind the iterator.
	 * Nicer way of accessing it beside operator *.
	 */
	val_t const &value() const override {
		return this->base()->second.value;
	}

	/**
	 * Get the key pointed to by this iterator.
	 */
	const key_t &key() const {
		return this->base()->first;
	}

};

}} // openage::curve
