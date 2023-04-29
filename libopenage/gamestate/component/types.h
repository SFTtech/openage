// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#pragma once


namespace openage::gamestate::component {

/**
 * Types of components.
 */
enum class component_t {
	// Internal
	POSITION,
	COMMANDQUEUE,
	OWNERSHIP,

	// API
	IDLE,
	TURN,
	MOVE,
	LIVE
};

} // namespace openage::gamestate::component
