// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once


namespace openage::gamestate::component::command {

/**
 * Command types.
 */
enum class command_t {
	NONE,

	CUSTOM,
	IDLE,
	MOVE,
	APPLY_EFFECT,
};

} // namespace openage::gamestate::component::command
