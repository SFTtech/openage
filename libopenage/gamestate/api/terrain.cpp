// Copyright 2023-2025 the openage authors. See copying.md for legal info.

#include "terrain.h"

#include <memory>

#include "gamestate/api/util.h"


namespace openage::gamestate::api {

bool APITerrain::is_terrain(const nyan::Object &obj) {
	return obj.extends("engine.util.terrain.Terrain");
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
		auto key = pair.first.get_value_ptr<nyan::ObjectValue>();
		auto value = pair.second.get_value_ptr<nyan::Int>();

		result.emplace(key->get_name(), value->get());
	}

	return result;
}

} // namespace openage::gamestate::api
