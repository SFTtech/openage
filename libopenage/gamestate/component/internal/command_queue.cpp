// Copyright 2021-2021 the openage authors. See copying.md for legal info.

#include "command_queue.h"
#include "../component_type.h"


namespace openage::gamestate::component {

component_t CommandQueue::get_component_type() const {
	return component_t::COMMANDQUEUE;
}

} // namespace openage::gamestate::component
