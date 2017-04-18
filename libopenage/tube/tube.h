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

}} // openage::tube
