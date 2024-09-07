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
	RESISTANCE,
	IDLE,
	LIVE,
	MOVE,
	TURN,

	// TODO
};

/**
 * Types of effects for API objects.
 */
enum class effect_t {
	CONTINUOUS_FLAC_DECREASE,
	CONTINUOUS_FLAC_INCREASE,
	CONTINUOUS_LURE,
	CONTINUOUS_TRAC_DECREASE,
	CONTINUOUS_TRAC_INCREASE,
	CONTINUOUS_TRPC_DECREASE,
	CONTINUOUS_TRPC_INCREASE,

	DISCRETE_CONVERT,
	DISCRETE_FLAC_DECREASE,
	DISCRETE_FLAC_INCREASE,
	DISCRETE_MAKE_HARVESTABLE,
	DISCRETE_SEND_TO_CONTAINER,
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
 * Types of properties for API effects.
 */
enum class effect_property_t {
	AREA,
	COST,
	DIPLOMATIC,
	PRIORITY,
};

/**
 * Types of properties for API patches.
 */
enum class patch_property_t {
	DIPLOMATIC,
};

} // namespace openage::gamestate::api
