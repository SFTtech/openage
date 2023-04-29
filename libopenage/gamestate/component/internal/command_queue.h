// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "gamestate/component/internal_component.h"

namespace openage::gamestate::component {

class CommandQueue : InternalComponent {
	// TODO: Add command queue member

	component_t get_type() const override;
};

} // namespace openage::gamestate::component
