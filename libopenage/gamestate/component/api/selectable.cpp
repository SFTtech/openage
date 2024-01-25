// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "selectable.h"


namespace openage::gamestate::component {

component_t Selectable::get_type() const {
	return component_t::SELECTABLE;
}

} // namespace openage::gamestate::component
