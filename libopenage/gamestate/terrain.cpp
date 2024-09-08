// Copyright 2018-2024 the openage authors. See copying.md for legal info.

#include "terrain.h"

#include <algorithm>
#include <array>
#include <cstddef>

#include "error/error.h"

#include "gamestate/terrain_chunk.h"
#include "renderer/render_factory.h"


namespace openage::gamestate {

Terrain::Terrain() :
	size{0, 0},
	chunks{} {
	// TODO: Get actual size of terrain.
}

Terrain::Terrain(const util::Vector2s &size,
                 std::vector<std::shared_ptr<TerrainChunk>> &&chunks) :
	size{size},
	chunks{std::move(chunks)} {
	// Check if chunk is correct
	coord::tile_delta current_offset{0, 0};
	util::Vector2s all_chunks_size{0, 0};
	if (this->chunks.size() > 0) {
		all_chunks_size = this->chunks[0]->get_size();
	}

	for (const auto &chunk : this->chunks) {
		auto chunk_size = chunk->get_size();

		// Check chunk delta
		auto chunk_offset = chunk->get_offset();
		if (current_offset != chunk_offset) [[unlikely]] {
			throw error::Error{ERR << "Chunk offset of chunk " << chunk->get_offset()
			                       << " does not match position in vector: " << chunk_offset
			                       << " (expected: " << current_offset << ")"};
		}

		current_offset.ne += chunk_size[0];

		// Wrap around to the next row
		if (current_offset.ne == static_cast<coord::tile_t>(size[0])) {
			current_offset.ne = 0;
			current_offset.se += chunk_size[1];
		}
		// Check if chunk size is correct
		else if (chunk_size != chunk_size) [[unlikely]] {
			throw error::Error{ERR << "Chunk size of chunk " << chunk->get_offset()
			                       << " is not equal to the first chunk size: " << chunk_size
			                       << " (expected: " << all_chunks_size << ")"};
		}
		else if (chunk_size[0] != chunk_size[1]) {
			throw error::Error{ERR << "Chunk size of chunk " << chunk->get_offset()
			                       << " is not square: " << chunk_size};
		}

		// Check terrain boundaries
		if (current_offset.ne > static_cast<coord::tile_t>(size[0])) {
			throw error::Error{ERR << "Width of chunk " << chunk->get_offset()
			                       << " exceeds terrain width: " << chunk_size[0]
			                       << " (max width: " << size[0] << ")"};
		}
		else if (current_offset.se > static_cast<coord::tile_t>(size[1])) {
			throw error::Error{ERR << "Height of chunk " << chunk->get_offset()
			                       << " exceeds terrain height: " << chunk_size[1]
			                       << " (max height: " << size[1] << ")"};
		}
	}
}

const util::Vector2s &Terrain::get_size() const {
	return this->size;
}

const util::Vector2s Terrain::get_chunks_size() const {
	auto chunk_size = this->chunks[0]->get_size();
	return {this->size[0] / chunk_size[0], this->size[1] / chunk_size[1]};
}

void Terrain::add_chunk(const std::shared_ptr<TerrainChunk> &chunk) {
	this->chunks.push_back(chunk);
}

const std::vector<std::shared_ptr<TerrainChunk>> &Terrain::get_chunks() const {
	return this->chunks;
}

const std::shared_ptr<TerrainChunk> &Terrain::get_chunk(size_t idx) const {
	return this->chunks.at(idx);
}

const std::shared_ptr<TerrainChunk> &Terrain::get_chunk(const coord::chunk &pos) const {
	size_t index = pos.ne + pos.se * this->size[0];

	return this->get_chunk(index);
}

void Terrain::attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory) {
	for (auto &chunk : this->get_chunks()) {
		auto render_entity = render_factory->add_terrain_render_entity(chunk->get_size(),
		                                                               chunk->get_offset());
		chunk->set_render_entity(render_entity);

		chunk->render_update(time::TIME_ZERO);
	}
}

} // namespace openage::gamestate
