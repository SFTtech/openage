// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include "util/path.h"


namespace openage::path::tests {

/**
 * Show the pathfinding functionality of the path module:
 *     - Cost field
 *     - Integration field
 *     - Flow field
 *
 * Visualizes the pathfinding results using our rendering backend.
 *
 * @param path Path to the project rootdir.
 */
void path_demo_0(const util::Path &path);


} // namespace openage::path::tests
