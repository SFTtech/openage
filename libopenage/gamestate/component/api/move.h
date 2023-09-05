// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "gamestate/component/api_component.h"
#include "gamestate/component/types.h"


namespace openage::gamestate::component {

class Move : public APIComponent {
public:
	using APIComponent::APIComponent;

	component_t get_type() const override;
};

} // namespace openage::gamestate::component
