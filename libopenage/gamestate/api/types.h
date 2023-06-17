// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once


namespace openage::gamestate::api {

/**
 * Types of abilities for API objects.
 */
enum class ability_t {
	IDLE,
	LIVE,
	MOVE,
	TURN,

	// TODO
};

/**
 * Types of properties for API abilities.
 */
enum class property_t {
	ANIMATED,
	ANIMATION_OVERRIDE,
	COMMAND_SOUND,
	EXECUTION_SOUND,
	DIPLOMATIC,
	LOCK,
};


} // namespace openage::gamestate::api
