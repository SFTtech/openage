// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <string>

#include "gamestate/component/internal/commands/base_command.h"
#include "gamestate/component/internal/commands/types.h"


namespace openage::gamestate::component::command {

/**
 * Custom command for everything that is not covered by the other commands.
 */
class CustomCommand : public Command {
public:
	/**
	 * Create a new custom command.
	 *
	 * @param id Command identifier.
	 */
	CustomCommand(const std::string &id);
	virtual ~CustomCommand() = default;

	inline command_t get_type() const override {
		return command_t::CUSTOM;
	}

	/**
	 * Get the command identifier.
	 *
	 * @return Command identifier.
	 */
	const std::string &get_id() const;

private:
	/**
	 * Command identifier.
	 */
	const std::string id;

	// TODO: Payload
};

} // namespace openage::gamestate::component::command
