// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#include "move.h"


namespace openage::gamestate::component::command {

Move::Move(const coord::phys3 &target) :
	target{target} {}

const coord::phys3 &Move::get_target() const {
	return this->target;
}

} // namespace openage::gamestate::component::command
