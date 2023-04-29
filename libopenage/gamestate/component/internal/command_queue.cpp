// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#include "command_queue.h"

#include "gamestate/component/types.h"


namespace openage::gamestate::component {

inline component_t CommandQueue::get_type() const {
	return component_t::COMMANDQUEUE;
}

} // namespace openage::gamestate::component
