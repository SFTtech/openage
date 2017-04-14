// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "tube.h"

#include <iterator>

namespace openage {
namespace tube {

template <class _Ty,
		  class container
          class container_iterator = container::const_iterator>
class timed_iterator : public std::iterator< std::bidirectional_iterator_tag, _Ty >
{
public:
	timed_iterator(typename container::const_iterator base,
	               tube_time_t reference_time) :
		base(base),
		now(reference_time) {}

	timed_iterator(const timed_iterator &rhs) :
		base(rhs.base),
		now(rhs.now) {}

	timed_iterator &operator = (const timed_iterator &rhs) {
		this->base = rhs.base;
		this->now = rhs.now;
		return *this;
	}

	timed_iterator operator ++() {
		++base;
		return *this;
	}

	timed_iterator operator ++(int) {
		auto tmp = *this;
		++base;
		return tmp;
	}

	timed_iterator operator --() {
		--base;
		return *this;
	}

	timed_iterator operator --(int) {
		auto tmp = *this;
		++base;
		return tmp;
	}

	const _Ty &operator *() const {
		return *base;
	}

	const _Ty *operator ->() const {
		return &**base;
	}

	tube_time_t time() {
		return (*this)->time;
	}

	bool operator ==(const timed_iterator<_Ty, container> &rhs) const {
		return base == rhs.base;
	}

	bool operator !=(const timed_iterator<_Ty, container> &rhs) const {
		return base != rhs.base;
	}

	bool valid() {
		return time() < to;
	}

	operator bool() {
		return valid();
	}

protected:
	typename container_iterator base;
	tube_time_t now;

};

}} // openage::tube
