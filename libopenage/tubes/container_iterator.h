// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "tube.h"

#include <iterator>

namespace openage {
namespace tube {

template <class _Ty,
		  class container,
		  class time_translator_f>
class timed_iterator : public std::iterator< std::bidirectional_iterator_tag, _Ty >
{
	typename container::const_iterator base;
	int64_t from, to;
	time_translator_f time_translator;

public:
	timed_iterator(typename container::const_iterator base, int64_t from, int64_t to, const time_translator_f& time_translator)
		: base(base)
		, from(from)
		, to(to)
		, time_translator(time_translator)
	{}

	timed_iterator(const timed_iterator &rhs)
		: base(rhs.base)
		, from(rhs.from)
		, to(rhs.to)
		, time_translator(rhs.time_translator)
	{}

	timed_iterator &operator =(const timed_iterator &rhs)
	{
		this->base = rhs.base;
		this->from = rhs.from;
		this->to = rhs.to;
		this->time_translator = rhs.time_translator;
		return *this;
	}

	timed_iterator &operator ++()
	{
		++base;
		return *this;
	}

	//We only want to Iterate forward - so maybe delete this?
	timed_iterator &operator --()
	{
		--base;
		return *this;
	}

	const _Ty &operator *() const
	{
		return *base;
	}

	const _Ty *operator ->() const
	{
		return &**base;
	}

	tube_time_t time() {
		return time_translator(**base);
	}

	bool operator ==(const timed_iterator<_Ty, container, time_translator_f> &rhs) const
	{
		return base == rhs.base;
	}

	bool operator !=(const timed_iterator<_Ty, container, time_translator_f> &rhs) const
	{
		return base != rhs.base;
	}

	bool valid()
	{
		return time() < to;
	}

	operator bool()
	{
		return valid();
	}
};

}} // openage::tube
