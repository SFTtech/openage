// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <optional>
#include <unordered_map>

#include <nyan/nyan.h>

#include "pathfinding/types.h"
#include "util/vector.h"

#include "renderer/render_factory.h"
#include "renderer/stages/camera/render_entity.h"


namespace openage {
namespace path {
class Pathfinder;
} // namespace path

namespace gamestate {
class GameState;
class Terrain;

class Map {
public:
	/**
	 * Create a new map from existing terrain.
	 *
	 * Initializes the pathfinder with the terrain path costs.
	 *
	 * @param state Game state.
	 * @param terrain Terrain object.
	 */
	Map(const std::shared_ptr<GameState> &state,
	    const std::shared_ptr<Terrain> &terrain);

	~Map() = default;

	/**
	 * Get the size of the map.
	 *
	 * @return Map size (width x height).
	 */
	const util::Vector2s &get_size() const;

	/**
	 * Get the terrain of the map.
	 *
	 * @return Terrain.
	 */
	const std::shared_ptr<Terrain> &get_terrain() const;

	/**
	 * Get the pathfinder for the map.
	 *
	 * @return Pathfinder.
	 */
	const std::shared_ptr<path::Pathfinder> &get_pathfinder() const;

	/**
	 * Get the grid ID associated with a nyan path grid object.
	 *
	 * @param path_grid Path grid object fqon.
	 *
	 * @return Grid ID.
	 */
	path::grid_id_t get_grid_id(const nyan::fqon_t &path_grid) const;

	const void set_camera_render_entity(std::shared_ptr<renderer::camera::RenderEntity> renderer_entity,
	                                    const std::optional<renderer::camera::CameraBoundaries> &boundaries = std::nullopt,
	                                    const time::time_t time = time::TIME_ZERO);

	const void upadate_camera_render_entity(const std::optional<renderer::camera::CameraBoundaries> &boundaries, const time::time_t time);

private:
	/**
	 * Terrain.
	 */
	std::shared_ptr<Terrain> terrain;

	/**
	 * Pathfinder.
	 */
	std::shared_ptr<path::Pathfinder> pathfinder;

	/**
	 * Lookup table for mapping path grid objects in nyan to grid indices.
	 */
	std::unordered_map<nyan::fqon_t, path::grid_id_t> grid_lookup;

	std::shared_ptr<renderer::camera::RenderEntity> camera_render_entity;
};

} // namespace gamestate
} // namespace openage
