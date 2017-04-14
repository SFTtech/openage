// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <iterator>
#include <limits>

#include "tube.h"

namespace openage {
namespace tube {

template <typename val_t>
class TubeIterator {
public:
	virtual val_t &value() = 0;
};

template<typename _T>
bool valid(const _T &, const tube_time_t &at);

template <class _T>
using _valid_function_t = bool (*)(const _T&, const tube_time_t &);

template <class key_t,
          class val_t,
          class container_t,
          _valid_function_t<val_t> valid_f = valid<val_t> >
class TubeMapFilterIterator :
	virtual public std::iterator<std::forward_iterator_tag, val_t >,
	public TubeIterator<val_t>
{
protected:
	//typedef typename std::iterator<std::forward_iterator_tag, val_t> iterator_t;
	typedef typename container_t::iterator iterator_t;
	iterator_t base;
	iterator_t container_end;

	tube_time_t from;
	tube_time_t to;
	tube_time_t now;

public:
	TubeMapFilterIterator(const iterator_t &base,
	                      const iterator_t &container_end,
	                      const tube_time_t &from,
	                      const tube_time_t &to) :
		base{base},
		container_end{container_end},
		from{from},
		to{to} {}
		now{from} {}

	TubeMapFilterIterator(const TubeMapFilterIterator &rhs) :
		base{rhs.base},
		container_end{rhs.container_end},
		from{rhs.from},
		to{rhs.to},
		now{rhs.now} {}

	TubeMapFilterIterator &operator =(const TubeMapFilterIterator &rhs) {
		this->base = rhs.base;
		this->container_end = rhs.container_end;
		this->from = rhs.from;
		this->to = rhs.to;
		return *this;
	}

	TubeMapFilterIterator &operator ++() {
		do {
			++this->base;
		} while (this->base != this->container_end &&
			(existent_from(this->base->second.value) < from ||
			 existent_until(this->base->second.value) > to));
		return *this;
	}

	//We only want to Iterate forward - so maybe delete this?
	//timed_iterator &operator --()
	//{
	//	--base;
	//	return *this;
	//}

	val_t &operator *() const {
		return this->base->second.value;
	}

	val_t *operator ->() const {
		return &**this;
	}

	virtual bool operator ==(const TubeMapFilterIterator<key_t, val_t, container_t> &rhs) const {
		return this->base == rhs.base;
	}

	virtual bool operator !=(const TubeMapFilterIterator<key_t, val_t, container_t> &rhs) const {
		return this->base != rhs.base;
	}

	virtual bool valid(const tube_time_t &time) {
		return valid_f(base->second.value, time);
	}

	val_t &value() override {
		return this->base->second.value;
	}

	const key_t &key() {
		return this->base->first;
	}
};

template<typename _T>
bool valid(const _T &t,
           const tube_time_t& time) {
	return existent_from(t) <= time && existent_until(t) > time;
}


}} // openage::tube
