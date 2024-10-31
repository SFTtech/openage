// Copyright 2022-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "coord/tile.h"
#include "util/vector.h"


namespace openage::renderer {
namespace terrain {
class TerrainRenderStage;
class RenderEntity;
} // namespace terrain

namespace world {
class WorldRenderStage;
class RenderEntity;
} // namespace world

/**
 * Creates render entities that push animation updates from the
 * engine to the renderer. Also registers the render entities with
 * the corresponding render stages.
 */
class RenderFactory {
public:
	/**
	 * Create a new factory for render entities.
	 *
	 * @param terrain_renderer Terrain renderer.
	 * @param world_renderer World renderer.
	 */
	RenderFactory(const std::shared_ptr<terrain::TerrainRenderStage> terrain_renderer,
	              const std::shared_ptr<world::WorldRenderStage> world_renderer);
	~RenderFactory() = default;

	/**
	 * Create a new terrain render entity and register it at the terrain renderer.
	 *
	 * Render entities for terrain are associated with chunks, so a new render entity
	 * will result in the creation of a new chunk in the renderer.
	 *
	 * Size/offset of the chunk in the game simulation should match size/offset
	 * in the renderer.
	 *
	 * @return Render entity for pushing terrain updates.
	 */
	std::shared_ptr<terrain::RenderEntity> add_terrain_render_entity(const util::Vector2s chunk_size,
	                                                                 const coord::tile_delta chunk_offset);

	/**
	 * Create a new world render entity and register it at the world renderer.
	 *
	 * @return Render entity for pushing terrain updates.
	 */
	std::shared_ptr<world::RenderEntity> add_world_render_entity();

private:
	/**
	 * Render stage for terrain drawing.
	 */
	std::shared_ptr<terrain::TerrainRenderStage> terrain_renderer;

	/**
	 * Render stage for game entity drawing.
	 */
	std::shared_ptr<world::WorldRenderStage> world_renderer;
};

} // namespace openage::renderer
