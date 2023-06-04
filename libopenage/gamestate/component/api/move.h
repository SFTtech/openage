// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "gamestate/component/api_component.h"

namespace openage::gamestate::component {

class Move : APIComponent {
	component_t get_type() const override;
};

} // namespace openage::gamestate::component
