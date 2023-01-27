// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#include "universe.h"

#include "gamestate/terrain.h"
#include "gamestate/world.h"
#include "renderer/entity_factory.h"

namespace openage::gamestate {

Universe::Universe(const util::Path &root_dir) :
	world{std::make_shared<World>(root_dir)} {
	// TODO
	auto texpath = root_dir / "assets" / "textures" / "test_terrain_tex.png";
	this->terrain = std::make_shared<Terrain>(texpath);
}

std::shared_ptr<World> Universe::get_world() {
	return this->world;
}

std::shared_ptr<Terrain> Universe::get_terrain() {
	return this->terrain;
}

void Universe::attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory) {
	this->render_factory = render_factory;

	// TODO: Notify entities somwhere else?
	auto terrain_render_entity = this->render_factory->add_terrain_render_entity();
	this->terrain->set_render_entity(terrain_render_entity);

	this->world->attach_renderer(render_factory);
}

} // namespace openage::gamestate
