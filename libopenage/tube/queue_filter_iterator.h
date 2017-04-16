// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <iterator>
#include <limits>

#include "tube.h"

namespace openage {
namespace tube {

template <typename _T>
class Queue;

template <class val_t,
          class container_t>
class TubeQueueFilterIterator :
	virtual public std::iterator<std::forward_iterator_tag, val_t >,
	public TubeIterator<val_t>
{
	friend class Queue<val_t>;
protected:
	typedef typename container_t::iterator iterator_t;
	iterator_t base;
	iterator_t container_end;

	tube_time_t from;
	tube_time_t to;

public:
	TubeQueueFilterIterator(const iterator_t &base,
	                        const iterator_t &container_end,
	                        const tube_time_t &from,
	                        const tube_time_t &to) :
		base{base},
		container_end{container_end},
		from{from},
		to{to} {}

	TubeQueueFilterIterator(const TubeQueueFilterIterator &rhs) :
		base{rhs.base},
		container_end{rhs.container_end},
		from{rhs.from},
		to{rhs.to} {}

	TubeQueueFilterIterator &operator =(const TubeQueueFilterIterator &rhs) {
		this->base = rhs.base;
		this->container_end = rhs.container_end;
		this->from = rhs.from;
		this->to = rhs.to;
		return *this;
	}

	TubeQueueFilterIterator &operator ++() {
		do {
			++this->base;
		} while (this->base != this->container_end &&
		         (this->base->time() < from ||
		          this->base->time() > to));
		return *this;
	}

	//We only want to Iterate forward - so maybe delete this?
	//timed_iterator &operator --()
	//{
	//	--base;
	//	return *this;
	//}

	val_t &operator *() const {
		return this->base->value;
	}

	val_t *operator ->() const {
		return &**this;
	}

	virtual bool operator ==(const TubeQueueFilterIterator<val_t, container_t> &rhs) const {
		return this->base == rhs.base;
	}

	virtual bool operator !=(const TubeQueueFilterIterator<val_t, container_t> &rhs) const {
		return this->base != rhs.base;
	}

	virtual bool valid() const {

		if (this->base != this->container_end) {
			return this->base->time() >= from && this->base->time() < to;
		}
		return false;
	}

	val_t &value() override {
		return this->base->value;
	}
};

}} // openage::tube
