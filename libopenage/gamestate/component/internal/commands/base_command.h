// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include "gamestate/component/internal/commands/types.h"


namespace openage::gamestate::component::command {

/**
 * Base interface for commands.
 */
class Command {
public:
	virtual ~Command() = default;

	/**
	 * Get the type of the command.
	 *
	 * @return Command type.
	 */
	virtual command_t get_type() const = 0;
};

} // namespace openage::gamestate::component::command
