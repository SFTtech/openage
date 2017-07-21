// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "curve.h"

namespace openage {
namespace curve {

/**
 * Default interface for curve containers
 */
template <typename val_t,
          class container_t,
          class iterator_t = typename container_t::const_iterator>
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
	explicit CurveIterator(const container_t *c):
		_base{},
		container{c},
		from{-std::numeric_limits<curve_time_t>::max()},
		to{+std::numeric_limits<curve_time_t>::max()} {}

	/** Default copy c'tor */
	CurveIterator (const CurveIterator &) = default;

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
		return this->_base == rhs._base;
	}

	/**
	 * For unequalness only the base iterator will be testet - not the timespans
	 * this is defined in.
	 */
	virtual bool operator !=(const CurveIterator<val_t, container_t> &rhs) const {
		return this->_base != rhs._base;
	}

	/**
	 * Advance to the next valid element.
	 */
	virtual CurveIterator<val_t, container_t> &operator ++() {
		do {
			++(this->_base);
		} while (this->container->end()._base != this->_base && !this->valid());
		return *this;
	}

	/**
	 * Access the underlying
	 */
	const iterator_t &base() const {
		return _base;
	}

	/**
	 * Access the lower end value of the defined time frame
	 */
	const curve_time_t &_from() const {
		return from;
	}

	/**
	 * Access the higher end value of the defined time frame
	 */
	const curve_time_t &_to() const {
		return to;
	}
protected:
	/**
	 * Can only be constructed from the referenced container
	 */
	CurveIterator(const iterator_t &base,
	              const container_t *container,
	              const curve_time_t &from,
	              const curve_time_t &to)  :
		_base{base},
		container{container},
		from{from},
		to{to} {}

protected:
	/// The iterator this is currently referring to.
	iterator_t _base;
	/// The base container.
	const container_t *container;

	/// The time, from where this iterator started to iterate.
	curve_time_t from;
	/// The time, to where this iterator will iterate.
	curve_time_t to;
};


}} // openage::curve
