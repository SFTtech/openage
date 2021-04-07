// Copyright 2021-2021 the openage authors. See copying.md for legal info.

#pragma once


namespace openage::gamestate::component {

/**
 * Types of components.
 */
enum class component_t {
    POSITION,
    COMMANDQUEUE,
    OWNERSHIP,
    IDLE,
    TURN,
    MOVE,
    LIVE
};

} // openage::gamestate::component
