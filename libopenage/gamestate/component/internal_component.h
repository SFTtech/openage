// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "gamestate/component/base_component.h"

namespace openage::gamestate::component {

/**
 * Interface for components that track ingame information about
 * a game entity, but don't use information from the nyan API.
 */
class InternalComponent : public Component {};

} // namespace openage::gamestate::component
