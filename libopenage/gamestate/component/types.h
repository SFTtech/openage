// Copyright 2021-2024 the openage authors. See copying.md for legal info.

#pragma once

#include "util/fixed_point.h"


namespace openage::gamestate::component {

/**
 * Type for attribute values.
 */
using attribute_value_t = util::FixedPoint<int64_t, 16>;

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
	RESISTANCE,
	IDLE,
	TURN,
	MOVE,
	SELECTABLE,
	LIVE,
	LINE_OF_SIGHT,
};

} // namespace openage::gamestate::component
