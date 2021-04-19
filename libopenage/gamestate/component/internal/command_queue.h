// Copyright 2021-2021 the openage authors. See copying.md for legal info.

#pragma once

#include "../internal_component.h"

namespace openage::gamestate::component {

class CommandQueue : InternalComponent {
	component_t get_component_type() const override;
};

} // namespace openage::gamestate::component
