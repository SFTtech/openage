// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "apply_effect.h"


namespace openage::gamestate::component::command {

ApplyEffect::ApplyEffect(const gamestate::entity_id_t &target) :
	target{target} {}

const gamestate::entity_id_t &ApplyEffect::get_target() const {
	return this->target;
}

} // namespace openage::gamestate::component::command
