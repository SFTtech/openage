// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#include "universe.h"

#include "gamestate/terrain.h"
#include "gamestate/world.h"
#include "renderer/render_factory.h"

namespace openage::gamestate {

Universe::Universe(const std::shared_ptr<GameState> &state) :
	world{std::make_shared<World>(state)} {
}

std::shared_ptr<World> Universe::get_world() {
	return this->world;
}

std::shared_ptr<Terrain> Universe::get_terrain() {
	return this->terrain;
}

void Universe::attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory) {
	this->render_factory = render_factory;

	this->world->attach_renderer(render_factory);
}

} // namespace openage::gamestate
