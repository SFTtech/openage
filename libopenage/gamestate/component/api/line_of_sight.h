// Copyright 2024-2025 the openage authors. See copying.md for legal info.

#pragma once

#include "gamestate/component/api_component.h"
#include "gamestate/component/types.h"


namespace openage::gamestate::component {

/**
 * Stores the line of sight information of a game entity.
 */
class LineOfSight final : public APIComponent {
public:
	using APIComponent::APIComponent;

	component_t get_type() const override;
};

} // namespace openage::gamestate::component
