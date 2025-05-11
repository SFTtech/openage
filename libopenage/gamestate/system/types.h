// Copyright 2023-2025 the openage authors. See copying.md for legal info.

#pragma once


namespace openage::gamestate::system {

/**
 * Associates a system function with an identifier.
 */
enum class system_id_t {
	NONE,

	// ability systems
	APPLY_EFFECT,

	IDLE,

	MOVE_COMMAND,
	MOVE_DEFAULT,

	ACTIVITY_ADVANCE,

	// tasks
	CLEAR_COMMAND_QUEUE,
	POP_COMMAND_QUEUE,
};

} // namespace openage::gamestate::system
