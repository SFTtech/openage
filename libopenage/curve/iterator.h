// Copyright 2017-2018 the openage authors. See copying.md for legal info.

#pragma once

#include "curve.h"

namespace openage::curve {

/**
 * Default interface for curve containers
 */
template <typename val_t,
          typename container_t,
          typename iterator_t=typename container_t::const_iterator>
class CurveIterator {
public:
	/**
	 * access the value of the iterator
	 */
	virtual const val_t &value() const = 0;

	/**
	 * Check if the iterator is still valid
	 * (this breaks from the stl - in the best way)
	 */
	virtual bool valid() const = 0;

	/**
	 * The iterator needs a reference to the container
	 */
	explicit CurveIterator(const container_t *c)
		:
		base{},
		container{c},
		from{-std::numeric_limits<time_t>::max()},
		to{+std::numeric_limits<time_t>::max()} {}

protected:
	/**
	 * Can only be constructed from the referenced container
	 */
	CurveIterator(const iterator_t &base,
	              const container_t *container,
	              const time_t &from,
	              const time_t &to)
		:
		base{base},
		container{container},
		from{from},
		to{to} {}

public:
	/** Default copy c'tor */
	CurveIterator (const CurveIterator &) = default;

	virtual ~CurveIterator() = default;

	/** Default assignment operator */
	CurveIterator<val_t, container_t, iterator_t> &operator= (
		const CurveIterator<val_t, container_t, iterator_t> &) = default;

	/** Dereference will call the virtual function */
	virtual const val_t &operator *() const {
		return this->value();
	}

	/** Dereference will call the virutal function */
	virtual const val_t *operator ->() const {
		return &this->value();
	}

	/**
	 * For equalness only the base iterator will be testet - not the timespans
	 * this is defined in.
	 */
	virtual bool operator ==(const CurveIterator<val_t, container_t> &rhs) const {
		return this->base == rhs.base;
	}

	/**
	 * For unequalness only the base iterator will be testet - not the timespans
	 * this is defined in.
	 */
	virtual bool operator !=(const CurveIterator<val_t, container_t> &rhs) const {
		return this->base != rhs.base;
	}

	/**
	 * Advance to the next valid element.
	 */
	virtual CurveIterator<val_t, container_t> &operator ++() {
		do {
			++this->base;
		} while (this->container->end().base != this->base and not this->valid());

		return *this;
	}

	/**
	 * Access the underlying
	 */
	const iterator_t &get_base() const {
		return base;
	}

	/**
	 * Access the lower end value of the defined time frame
	 */
	const time_t &get_from() const {
		return from;
	}

	/**
	 * Access the higher end value of the defined time frame
	 */
	const time_t &get_to() const {
		return to;
	}

protected:
	/// The iterator this is currently referring to.
	iterator_t base;

	/// The base container.
	const container_t *container;

	/// The time, from where this iterator started to iterate.
	time_t from;

	/// The time, to where this iterator will iterate.
	time_t to;
};


} // openage::curve
