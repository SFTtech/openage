// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <cassert>

namespace qtsdl {

template<typename T, typename U>
T checked_static_cast(U *u) {
	assert(dynamic_cast<T>(u));
	return static_cast<T>(u);
}

} // namespace qtsdl
