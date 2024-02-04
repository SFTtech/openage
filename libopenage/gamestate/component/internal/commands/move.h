// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include "coord/phys.h"
#include "gamestate/component/internal/commands/base_command.h"
#include "gamestate/component/internal/commands/types.h"


namespace openage::gamestate::component::command {

/**
 * Command for moving to a target position.
 */
class MoveCommand : public Command {
public:
	/**
	 * Creates a new move command.
	 *
	 * @param target Target position coordinates.
	 */
	MoveCommand(const coord::phys3 &target);
	virtual ~MoveCommand() = default;

	inline command_t get_type() const override {
		return command_t::MOVE;
	}

	/**
	 * Get the target position.
	 *
	 * @return Target position coordinates.
	 */
	const coord::phys3 &get_target() const;

private:
	/**
	 * Target position.
	 */
	const coord::phys3 target;
};

} // namespace openage::gamestate::component::command
