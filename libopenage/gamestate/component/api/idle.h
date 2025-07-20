// Copyright 2021-2025 the openage authors. See copying.md for legal info.

#pragma once

#include "gamestate/component/api_component.h"
#include "gamestate/component/types.h"

namespace openage::gamestate::component {

/**
 * Represents an idle state of a game entity, i.e. when it is not
 * performing any action or command.
 */
class Idle final : public APIComponent {
public:
	using APIComponent::APIComponent;

	component_t get_type() const override;
};

} // namespace openage::gamestate::component
