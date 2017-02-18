// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "base.h"

namespace openage {
namespace tube {

template<typename _T>
class SimpleType {
protected:
	tubebase<_T> container;
	mutable tubeelement<_T> *e_now;
	mutable tube_time_t now;

public:
	SimpleType();

	// Reader mode
	virtual void set_now(const tube_time_t &t) const;

	virtual _T get() const = 0;
	virtual _T get(const tube_time_t &t) const;

	virtual _T operator ()() const {
		return std::move(get());
	}

	virtual _T operator ()(const tube_time_t &now) {
		return std::move(get(now));
	}

	virtual bool needs_update(const tube_time_t &at);
public:
	// Inserter mode
	void set_drop(const tube_time_t &at, const _T &value);
	void set_insert(const tube_time_t &at, const _T &value);
};

template <typename _T>
SimpleType<_T>::SimpleType() :
	e_now(nullptr),
	now()
{}

template <typename _T>
void SimpleType<_T>::set_now(const tube_time_t &t) const {
	now = t;
	e_now = container.last(t, e_now);
}

template <typename _T>
_T SimpleType<_T>::get(const tube_time_t &t) const {
	set_now(t);
	return get();
}

template <typename _T>
void SimpleType<_T>::set_drop(const tube_time_t &at, const _T &value) {
	container.erase_after(container.last(at, e_now));
	container.create(at, value);
}

template <typename _T>
void SimpleType<_T>::set_insert(const tube_time_t &at, const _T &value) {
	container.create(at, value);
}

template <typename _T>
bool SimpleType<_T>::needs_update(const tube_time_t &at) {
	auto e = container.last(at, e_now); //TODO take container.end as a hint?
	if (e->time > at || e->next == nullptr || e->next->time > at) {
		return true;
	} else {
		return false;
	}
}

}} // openage::tube
