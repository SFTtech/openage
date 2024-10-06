// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#include "terrain.h"

#include <memory>

#include "gamestate/api/util.h"


namespace openage::gamestate::api {

bool APITerrain::is_terrain(const nyan::Object &obj) {
	nyan::fqon_t immediate_parent = obj.get_parents()[0];
	return immediate_parent == "engine.util.terrain.Terrain";
}

const std::string APITerrain::get_terrain_path(const nyan::Object &terrain) {
	nyan::Object terrain_texture_obj = terrain.get_object("Terrain.terrain_graphic");
	std::string terrain_path = terrain_texture_obj.get_file("Terrain.sprite");

	return resolve_file_path(terrain, terrain_path);
}

const std::unordered_map<nyan::fqon_t, int> APITerrain::get_path_costs(const nyan::Object &terrain) {
	std::unordered_map<nyan::fqon_t, int> result;

	nyan::dict_t path_costs = terrain.get_dict("Terrain.path_costs");
	for (const auto &pair : path_costs) {
		auto key = std::dynamic_pointer_cast<nyan::ObjectValue>(pair.first.get_ptr());
		auto value = std::dynamic_pointer_cast<nyan::Int>(pair.second.get_ptr());

		result.emplace(key->get_name(), value->get());
	}

	return result;
}

} // namespace openage::gamestate::api
