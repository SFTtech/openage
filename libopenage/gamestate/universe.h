// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

namespace openage::gamestate {
class Terrain;
class World;


/**
 * Entity for managing the "physical" game world entities (units, buildings, etc.) as well as
 * conceptual entities (players, resources, ...).
 */
class Universe {
public:
	Universe() = default;
	~Universe() = default;

private:
	std::shared_ptr<World> world;
	std::shared_ptr<Terrain> terrain;
};

} // namespace openage::gamestate
