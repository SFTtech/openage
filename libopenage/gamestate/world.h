// Copyright 2018-2018 the openage authors. See copying.md for legal info.

#pragma once

#include "util/path.h"

namespace openage {

namespace renderer {
class RenderFactory;
}

namespace gamestate {
class GameEntity;

/**
 * Entity for managing "physical" things (units, buildings) inside
 * the game.
 */
class World {
public:
	World(const util::Path &root_dir);
	~World() = default;

	void attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory);

private:
	std::vector<std::shared_ptr<GameEntity>> game_entities;

	std::shared_ptr<renderer::RenderFactory> render_factory;
};

} // namespace gamestate
} // namespace openage
