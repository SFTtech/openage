// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

namespace openage::renderer {
namespace terrain {
class TerrainRenderer;
class TerrainRenderEntity;
} // namespace terrain

namespace world {
class WorldRenderer;
class WorldRenderEntity;
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
	RenderFactory(const std::shared_ptr<terrain::TerrainRenderer> terrain_renderer,
	              const std::shared_ptr<world::WorldRenderer> world_renderer);
	~RenderFactory() = default;

	/**
     * Create a new terrain render entity and register it at the terrain renderer.
     *
     * @return Render entity for pushing terrain updates.
     */
	std::shared_ptr<terrain::TerrainRenderEntity> add_terrain_render_entity();

	/**
     * Create a new world render entity and register it at the world renderer.
     *
     * @return Render entity for pushing terrain updates.
     */
	std::shared_ptr<world::WorldRenderEntity> add_world_render_entity();

private:
	/**
     * Render stage for terrain drawing.
     */
	std::shared_ptr<terrain::TerrainRenderer> terrain_renderer;

	/**
     * Render stage for game entity drawing.
     */
	std::shared_ptr<world::WorldRenderer> world_renderer;
};

} // namespace openage::renderer
