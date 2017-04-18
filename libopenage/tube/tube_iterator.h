// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "tube.h"

namespace openage {
namespace tube {

template <typename val_t,
          class container_t,
          class iterator_t = typename container_t::iterator>
class TubeIterator :
	public std::iterator<std::forward_iterator_tag, val_t >
{
public:
	virtual val_t &value() const = 0;
	virtual bool valid() const = 0;

	TubeIterator(const TubeIterator &rhs) :
		base{rhs.base},
		container_end{rhs.container_end},
		from{rhs.from},
		to{rhs.to} {}

	TubeIterator &operator =(const TubeIterator &rhs) {
		this->base = rhs.base;
		this->container_end = rhs.container_end;
		this->from = rhs.from;
		this->to = rhs.to;
		return *this;
	}

	virtual val_t &operator *() const {
		return this->value();
	}

	virtual val_t *operator ->() const {
		return &this->value();
	}

	/**
	 * For equalness only the base iterator will be testet - not the timespans
	 * this is defined in.
	 */
	virtual bool operator ==(const TubeIterator<val_t, container_t> &rhs) const {
		return this->base == rhs.base;
	}

	/**
	 * For unequalness only the base iterator will be testet - not the timespans
	 * this is defined in.
	 */
	virtual bool operator !=(const TubeIterator<val_t, container_t> &rhs) const {
		return this->base != rhs.base;
	}

	/**
	 * Advance to the next valid element.
	 */
	virtual TubeIterator &operator ++() {
		do {
			++this->base;
		} while (this->base != this->container_end && !this->valid());
		return *this;
	}

protected:
	TubeIterator (const iterator_t &base,
	              const iterator_t &container_end,
	              const tube_time_t &from,
	              const tube_time_t &to)  :
		base{base},
		container_end{container_end},
		from{from},
		to{to} {}

protected:
	/// The iterator this is currently referring to.
	iterator_t base;
	/// The iterator to the containers end.
	iterator_t container_end;

	/// The time, from where this iterator started to iterate.
	tube_time_t from;
	/// The time, to where this iterator will iterate.
	tube_time_t to;
};

}} // openage::tube
