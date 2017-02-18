// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <iterator>
#include <limits>

#include "tube.h"

namespace openage {
namespace tube {

template <typename val_t>
class tube_iterator{
public:
	virtual val_t &value() = 0;
	virtual bool valid() = 0;
};

template <class key_t,
		  class val_t,
		  class container_t>
class tube_filter_iterator :
	public std::iterator< std::bidirectional_iterator_tag, std::pair<key_t, val_t> >,
	public tube_iterator<val_t>
{
protected:
//	typename container_t::const_iterator base;
	typename container_t::iterator base;

	tube_time_t now;

public:
	tube_filter_iterator(const typename container_t::const_iterator &base, const tube_time_t &now)
		: base(base)
		, now(now)
	{}

	tube_filter_iterator(const tube_filter_iterator &rhs)
		: base(rhs.base)
		, now(rhs.now)
	{}

	tube_filter_iterator &operator =(const tube_filter_iterator &rhs)
	{
		this->base = rhs.base;
		this->now = rhs.now;
		return *this;
	}

	tube_filter_iterator &operator ++()
	{
		do {
			++base;
		} while (base->second.alive > time && base->second.dead < time);

		return *this;
	}

	//We only want to Iterate forward - so maybe delete this?
	//timed_iterator &operator --()
	//{
	//	--base;
	//	return *this;
	//}

	std::pair<key_t &, val_t &> &operator *() const
	{
		return *base;
	}

	std::pair<key_t &, val_t &> *operator ->() const
	{
		return &**this;
	}

	virtual bool operator ==(const tube_filter_iterator<key_t, val_t, container_t> &rhs) const
	{
		return rhs.now ==  std::numeric_limits<tube_time_t>::infinity()
			&& base == rhs.base;
	}

	virtual bool operator !=(const tube_filter_iterator<key_t, val_t, container_t> &rhs) const
	{
		return rhs.now !=  std::numeric_limits<tube_time_t>::infinity()
			&& base != rhs.base;
	}

	virtual bool valid()
	{
		return base->second.alive > now;
	}

	operator bool()
	{
		return valid();
	}
};

template <typename key_t, typename val_t, typename container_t>
class tube_filter_end_iterator : public tube_filter_iterator<key_t, val_t, container_t> {
public:
	tube_filter_end_iterator(const typename container_t::const_iterator &base)
		: tube_filter_iterator<key_t, val_t, container_t>(base, std::numeric_limits<tube_time_t>::infinity())
	{}

	virtual bool valid() {
		return false;
	}
};

}} // openage::tube
