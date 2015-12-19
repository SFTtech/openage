// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include <functional>

#include "action.h"

namespace openage {
namespace input {


int action_hash::operator()(const action_t& a) const {
	return std::hash<int>()(a.key);
}

bool action_t::operator==(const action_t& other) const {
	return key == other.key;
}


}} // openage::input
