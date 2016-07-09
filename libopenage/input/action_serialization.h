// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <iostream>

#include "action.h"
#include "event_serialization.h"

namespace openage {
namespace input {

struct deserialized_action_arg_t {
	DeserializedEvent e;

	coord::window mouse;
	coord::window_delta motion;

	std::vector<action_id_t> hints;
};

action_arg_t to_action(const deserialized_action_arg_t &action);

std::ostream& operator<<(std::ostream &o, std::tuple<const action_arg_t&, const ActionManager&> action);
std::istream& operator>>(std::istream &i, std::tuple<deserialized_action_arg_t&, const ActionManager&> action);

}} // openage::input
