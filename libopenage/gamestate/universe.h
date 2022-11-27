// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "util/path.h"

namespace openage {

namespace renderer {
class RenderFactory;
}

namespace gamestate {
class Terrain;
class World;

/**
 * Entity for managing the "physical" game world entities (units, buildings, etc.) as well as
 * conceptual entities (players, resources, ...).
 */
class Universe {
public:
	Universe(const util::Path &root_dir);
	~Universe() = default;

	std::shared_ptr<World> get_world();
	std::shared_ptr<Terrain> get_terrain();

	void attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory);

private:
	std::shared_ptr<World> world;
	std::shared_ptr<Terrain> terrain;

	std::shared_ptr<renderer::RenderFactory> render_factory;
};

} // namespace gamestate
} // namespace openage
