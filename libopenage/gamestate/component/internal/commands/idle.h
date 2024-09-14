// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include "gamestate/component/internal/commands/base_command.h"
#include "gamestate/component/internal/commands/types.h"


namespace openage::gamestate::component::command {

/**
 * Command for idling (TODO: rename to Stop?).
 */
class Idle final : public Command {
public:
	Idle() = default;
	virtual ~Idle() = default;

	inline command_t get_type() const override {
		return command_t::IDLE;
	}
};

} // namespace openage::gamestate::component::command
