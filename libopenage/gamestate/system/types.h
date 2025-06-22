// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once


namespace openage::gamestate::system {

/**
 * Associates a system function with an identifier.
 */
enum class system_id_t {
	NONE,

	APPLY_EFFECT,

	IDLE,

	MOVE_COMMAND,
	MOVE_DEFAULT,

	ACTIVITY_ADVANCE,
};

} // namespace openage::gamestate::system
