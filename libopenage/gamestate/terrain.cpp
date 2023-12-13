// Copyright 2018-2023 the openage authors. See copying.md for legal info.

#include "terrain.h"

#include <algorithm>
#include <array>
#include <cstddef>

#include "gamestate/terrain_chunk.h"
#include "renderer/render_factory.h"


namespace openage::gamestate {

Terrain::Terrain() :
	size{0, 0},
	chunks{} {
	// TODO: Get actual size of terrain.
}

void Terrain::add_chunk(const std::shared_ptr<TerrainChunk> &chunk) {
	this->chunks.push_back(chunk);
}

const std::vector<std::shared_ptr<TerrainChunk>> &Terrain::get_chunks() const {
	return this->chunks;
}

void Terrain::attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory) {
	for (auto &chunk : this->get_chunks()) {
		auto render_entity = render_factory->add_terrain_render_entity(chunk->get_size(),
		                                                               chunk->get_offset());
		chunk->set_render_entity(render_entity);

		chunk->render_update(time::time_t::zero());
	}
}

} // namespace openage::gamestate
