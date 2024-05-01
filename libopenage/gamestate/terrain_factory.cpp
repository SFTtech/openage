// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#include "terrain_factory.h"

#include <mutex>

#include <nyan/nyan.h>

#include "gamestate/api/terrain.h"
#include "gamestate/terrain.h"
#include "gamestate/terrain_chunk.h"
#include "gamestate/terrain_tile.h"
#include "renderer/render_factory.h"
#include "renderer/stages/terrain/render_entity.h"
#include "time/time.h"

#include "assets/mod_manager.h"
#include "gamestate/game_state.h"


namespace openage::gamestate {

// TODO: Remove test terrain references
static const std::string test_terrain_path = "../test/textures/test_terrain.terrain";
static const std::string test_terrain_path2 = "../test/textures/test_terrain2.terrain";

static const std::vector<nyan::fqon_t> aoe1_test_terrain = {};
static const std::vector<nyan::fqon_t> de1_test_terrain = {};
static const std::vector<nyan::fqon_t> aoe2_test_terrain = {
	"aoe2_base.data.terrain.foundation.foundation.Foundation",
	"aoe2_base.data.terrain.grass.grass.Grass",
	"aoe2_base.data.terrain.dirt.dirt.Dirt",
	"aoe2_base.data.terrain.water.water.Water",
};
static const std::vector<nyan::fqon_t> de2_test_terrain = {};
static const std::vector<nyan::fqon_t> hd_test_terrain = {
	"hd_base.data.terrain.foundation.foundation.Foundation",
	"hd_base.data.terrain.grass.grass.Grass",
	"hd_base.data.terrain.dirt.dirt.Dirt",
	"hd_base.data.terrain.water.water.Water",
};
static const std::vector<nyan::fqon_t> swgb_test_terrain = {
	"swgb_base.data.terrain.foundation.foundation.Foundation",
	"swgb_base.data.terrain.grass2.grass2.Grass2",
	"swgb_base.data.terrain.desert0.desert0.Desert0",
	"swgb_base.data.terrain.water1.water1.Water1",
};
static const std::vector<nyan::fqon_t> trial_test_terrain = {};

// TODO: Remove hardcoded test texture references
static std::vector<nyan::fqon_t> test_terrains; // declare static so we only have to do this once

void build_test_terrains(const std::shared_ptr<GameState> &gstate) {
	auto modpack_ids = gstate->get_mod_manager()->get_load_order();
	for (auto &modpack_id : modpack_ids) {
		if (modpack_id == "aoe1_base") {
			test_terrains.insert(test_terrains.end(),
			                     aoe1_test_terrain.begin(),
			                     aoe1_test_terrain.end());
		}
		else if (modpack_id == "de1_base") {
			test_terrains.insert(test_terrains.end(),
			                     de1_test_terrain.begin(),
			                     de1_test_terrain.end());
		}
		else if (modpack_id == "aoe2_base") {
			test_terrains.insert(test_terrains.end(),
			                     aoe2_test_terrain.begin(),
			                     aoe2_test_terrain.end());
		}
		else if (modpack_id == "de2_base") {
			test_terrains.insert(test_terrains.end(),
			                     de2_test_terrain.begin(),
			                     de2_test_terrain.end());
		}
		else if (modpack_id == "hd_base") {
			test_terrains.insert(test_terrains.end(),
			                     hd_test_terrain.begin(),
			                     hd_test_terrain.end());
		}
		else if (modpack_id == "swgb_base") {
			test_terrains.insert(test_terrains.end(),
			                     swgb_test_terrain.begin(),
			                     swgb_test_terrain.end());
		}
		else if (modpack_id == "trial_base") {
			test_terrains.insert(test_terrains.end(),
			                     trial_test_terrain.begin(),
			                     trial_test_terrain.end());
		}
	}
}

std::shared_ptr<Terrain> TerrainFactory::add_terrain() {
	// TODO: Replace this with a proper terrain generator.
	auto terrain = std::make_shared<Terrain>();

	return terrain;
}

std::shared_ptr<TerrainChunk> TerrainFactory::add_chunk(const std::shared_ptr<GameState> &gstate,
                                                        const util::Vector2s size,
                                                        const coord::tile_delta offset) {
	std::string terrain_info_path;

	// TODO: Remove test texture references
	// ==========
	std::optional<nyan::Object> terrain_obj;
	if (test_terrains.empty()) {
		build_test_terrains(gstate);
	}

	static size_t test_terrain_index = 0;
	if (not test_terrains.empty()) {
		// use one of the modpack terrain textures
		if (test_terrain_index >= test_terrains.size()) {
			test_terrain_index = 0;
		}
		terrain_obj = gstate->get_db_view()->get_object(test_terrains[test_terrain_index]);
		terrain_info_path = api::APITerrain::get_terrain_path(terrain_obj.value());

		test_terrain_index += 1;
	}
	else {
		// use a test texture
		if (test_terrain_index >= test_terrains.size()) {
			test_terrain_index = 0;
		}
		terrain_info_path = test_terrain_path;

		test_terrain_index += 1;
	}
	// ==========

	// fill the chunk with tiles
	std::vector<TerrainTile> tiles{};
	tiles.reserve(size[0] * size[1]);
	for (size_t i = 0; i < size[0] * size[1]; ++i) {
		tiles.push_back({terrain_obj, terrain_info_path, terrain_elevation_t::zero()});
	}

	auto chunk = std::make_shared<TerrainChunk>(size, offset, std::move(tiles));

	if (this->render_factory) {
		auto render_entity = this->render_factory->add_terrain_render_entity(size, offset);
		chunk->set_render_entity(render_entity);

		chunk->render_update(time::TIME_ZERO);
	}

	return chunk;
}

void TerrainFactory::attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory) {
	std::unique_lock lock{this->mutex};

	this->render_factory = render_factory;
}

} // namespace openage::gamestate
