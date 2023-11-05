// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "terrain.h"

#include <memory>


namespace openage::gamestate::api {

bool APITerrain::is_terrain(const nyan::Object &obj) {
	nyan::fqon_t immediate_parent = obj.get_parents()[0];
	return immediate_parent == "engine.util.terrain.Terrain";
}

const std::string APITerrain::get_terrain_path(const nyan::Object &terrain) {
	nyan::Object terrain_texture_obj = terrain.get_object("Terrain.terrain_graphic");
	std::string sprite_path = terrain_texture_obj.get_text("TerrainTexture.sprite");

	return sprite_path;
}

} // namespace openage::gamestate::api
