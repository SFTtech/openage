// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

namespace openage {
namespace tube {

typedef float tube_time_t;

template <class _T>
tube_time_t time(const _T &t) {
	return t.time();
}

template <class _T>
tube_time_t existent_from (const _T &t) {
	return t.existent_from();
}

template <class _T>
tube_time_t existent_until (const _T &t) {
	return t.existent_until();
}

template <typename val_t>
class TubeIterator {
public:
	virtual val_t &value() = 0;
};

template<typename _T>
bool valid(const _T &, const tube_time_t &at);

template <class _T>
using _valid_function_t = bool (*)(const _T&, const tube_time_t &);

template<typename _T>
bool valid(const _T &t,
           const tube_time_t& time) {
	return existent_from(t) <= time && existent_until(t) > time;
}



}} // openage::tube
