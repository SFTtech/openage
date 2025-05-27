// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include "gamestate/component/internal/commands/base_command.h"
#include "gamestate/component/internal/commands/types.h"
#include "gamestate/types.h"


namespace openage::gamestate::component::command {

/**
 * Command for applying effects to a game entity.
 */
class ApplyEffect final : public Command {
public:
	/**
	 * Creates a new apply effect command.
	 *
	 * @param target Target game entity ID.
	 */
	ApplyEffect(const gamestate::entity_id_t &target);
	virtual ~ApplyEffect() = default;

	inline command_t get_type() const override {
		return command_t::APPLY_EFFECT;
	}

	/**
	 * Get the ID of the game entity targeted by the command.
	 *
	 * @return ID of the targeted game entity.
	 */
	const gamestate::entity_id_t &get_target() const;

private:
	/**
	 * Target position.
	 */
	const gamestate::entity_id_t target;
};

} // namespace openage::gamestate::component::command
