// Copyright 2021-2025 the openage authors. See copying.md for legal info.

#pragma once

#include "gamestate/component/api_component.h"
#include "gamestate/component/types.h"


namespace openage::gamestate::component {

/**
 * Stores the movement information of a game entity.
 */
class Move final : public APIComponent {
public:
	using APIComponent::APIComponent;

	component_t get_type() const override;
};

} // namespace openage::gamestate::component
