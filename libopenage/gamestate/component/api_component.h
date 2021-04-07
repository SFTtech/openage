// Copyright 2021-2021 the openage authors. See copying.md for legal info.

#pragma once

#include <nyan/nyan.h>

#include "base_component.h"
#include "../../curve/discrete.h"

namespace openage::gamestate::component {

/**
 * Component that is represented by a nyan object from the engine.ability
 * namespace in the openage modding API.
 */
class APIComponent : Component {
public:

    APIComponent(nyan::Object &ability,
                 const bool enabled);

    /**
     * nyan object holding the data for the component.
     */
    nyan::Object ability;

    /**
     * Determines if the component is available to its game entity.
     */
    curve::Discrete<bool> enabled;

};

} // openage::gamestate::component
