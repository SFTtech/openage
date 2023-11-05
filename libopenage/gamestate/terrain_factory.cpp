// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "terrain_factory.h"

#include <mutex>

#include "gamestate/terrain.h"
#include "gamestate/terrain_chunk.h"
#include "renderer/render_factory.h"
#include "renderer/stages/terrain/terrain_render_entity.h"
#include "time/time.h"


namespace openage::gamestate {

std::shared_ptr<Terrain> TerrainFactory::add_terrain() {
	// TODO: Replace this with a proper terrain generator.
	auto terrain = std::make_shared<Terrain>();

	return terrain;
}

std::shared_ptr<TerrainChunk> TerrainFactory::add_chunk(const util::Vector2s size,
                                                        const util::Vector2s offset) {
	auto chunk = std::make_shared<TerrainChunk>(size, offset);

	if (this->render_factory) {
		auto render_entity = this->render_factory->add_terrain_render_entity();
		chunk->set_render_entity(render_entity);

		chunk->render_update(time::time_t::zero(),
		                     "../test/textures/test_terrain.terrain");
	}

	return chunk;
}

void TerrainFactory::attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory) {
	std::unique_lock lock{this->mutex};

	this->render_factory = render_factory;
}

} // namespace openage::gamestate
