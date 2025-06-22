// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once


namespace openage::gamestate::api {

/**
 * Types of abilities for API objects.
 */
enum class ability_t {
	APPLY_CONTINUOUS_EFFECT,
	APPLY_DISCRETE_EFFECT,
	RANGED_CONTINUOUS_EFFECT,
	RANGED_DISCRETE_EFFECT,
	IDLE,
	LIVE,
	MOVE,
	TURN,

	// TODO
};

/**
 * Types of properties for API abilities.
 */
enum class ability_property_t {
	ANIMATED,
	ANIMATION_OVERRIDE,
	COMMAND_SOUND,
	EXECUTION_SOUND,
	DIPLOMATIC,
	LOCK,
};

/**
 * Types of properties for API patches.
 */
enum class patch_property_t {
	DIPLOMATIC,
};

} // namespace openage::gamestate::api
