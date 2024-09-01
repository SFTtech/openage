// Copyright 2021-2024 the openage authors. See copying.md for legal info.

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
	ACTIVITY,

	// API
	APPLY_EFFECT,
	IDLE,
	TURN,
	MOVE,
	SELECTABLE,
	LIVE
};

} // namespace openage::gamestate::component
