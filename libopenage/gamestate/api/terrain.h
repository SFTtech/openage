// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <string>

#include <nyan/nyan.h>


namespace openage::gamestate::api {

class APITerrain {
public:
	/**
	 * Check if a nyan object is a terrain (type == \p engine.util.terrain.Terrain).
	 *
	 * @param obj nyan object handle.
	 *
	 * @return true if the object is a terrain, else false.
	 */
	static bool is_terrain(const nyan::Object &obj);

	/**
	 * Get the terrain path of a terrain.
	 *
	 * The path is relative to the directory the modpack is mounted in.
	 *
	 * @param terrain \p Terrain nyan object (type == \p engine.util.terrain.Terrain).
	 *
	 * @return Relative path to the terrain file.
	 */
	static const std::string get_terrain_path(const nyan::Object &terrain);
};

} // namespace openage::gamestate::api
