// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "move.h"


namespace openage::gamestate::component::command {

MoveCommand::MoveCommand(const coord::phys3 &target) :
	target{target} {}

const coord::phys3 &MoveCommand::get_target() const {
	return this->target;
}

} // namespace openage::gamestate::component::command
