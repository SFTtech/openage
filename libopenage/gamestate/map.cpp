// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "map.h"

#include <nyan/nyan.h>

#include "gamestate/api/terrain.h"
#include "gamestate/game_state.h"
#include "gamestate/terrain.h"
#include "gamestate/terrain_chunk.h"
#include "pathfinding/cost_field.h"
#include "pathfinding/grid.h"
#include "pathfinding/pathfinder.h"
#include "pathfinding/sector.h"


namespace openage::gamestate {
Map::Map(const std::shared_ptr<GameState> &state,
         const std::shared_ptr<Terrain> &terrain) :
	terrain{terrain},
	pathfinder{std::make_shared<path::Pathfinder>()},
	grid_lookup{} {
	// Create a grid for each path type
	// TODO: This is non-deterministic because of the unordered set. Is this a problem?
	auto nyan_db = state->get_db_view();
	std::unordered_set<nyan::fqon_t> path_types = nyan_db->get_obj_children_all("engine.util.path_type.PathType");
	size_t grid_idx = 0;
	auto chunk_size = this->terrain->get_chunk(0)->get_size();
	auto side_length = std::max(chunk_size[0], chunk_size[1]);
	auto grid_size = this->terrain->get_chunks_size();
	for (const auto &path_type : path_types) {
		auto grid = std::make_shared<path::Grid>(grid_idx, grid_size, side_length);
		this->pathfinder->add_grid(grid);

		this->grid_lookup.emplace(path_type, grid_idx);
		grid_idx += 1;
	}

	// Set path costs
	for (size_t chunk_idx = 0; chunk_idx < this->terrain->get_chunks().size(); ++chunk_idx) {
		auto chunk_terrain = this->terrain->get_chunk(chunk_idx);
		for (size_t tile_idx = 0; tile_idx < chunk_terrain->get_tiles().size(); ++tile_idx) {
			auto tile = chunk_terrain->get_tile(tile_idx);
			auto path_costs = api::APITerrain::get_path_costs(tile.terrain);

			for (const auto &path_cost : path_costs) {
				auto grid_id = this->grid_lookup.at(path_cost.first);
				auto grid = this->pathfinder->get_grid(grid_id);

				auto sector = grid->get_sector(chunk_idx);
				auto cost_field = sector->get_cost_field();
				cost_field->set_cost(tile_idx, path_cost.second, time::TIME_ZERO);
			}
		}
	}

	// Connect sectors with portals
	for (const auto &path_type : this->grid_lookup) {
		auto grid = this->pathfinder->get_grid(path_type.second);
		grid->init_portals();
		grid->init_portal_nodes();
	}
}

const util::Vector2s &Map::get_size() const {
	return this->terrain->get_size();
}

const std::shared_ptr<Terrain> &Map::get_terrain() const {
	return this->terrain;
}

const std::shared_ptr<path::Pathfinder> &Map::get_pathfinder() const {
	return this->pathfinder;
}

path::grid_id_t Map::get_grid_id(const nyan::fqon_t &path_grid) const {
	return this->grid_lookup.at(path_grid);
}

} // namespace openage::gamestate
