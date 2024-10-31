// Copyright 2022-2024 the openage authors. See copying.md for legal info.

#include "render_factory.h"

#include "coord/phys.h"
#include "renderer/stages/terrain/render_entity.h"
#include "renderer/stages/terrain/render_stage.h"
#include "renderer/stages/world/render_entity.h"
#include "renderer/stages/world/render_stage.h"

namespace openage::renderer {
RenderFactory::RenderFactory(const std::shared_ptr<terrain::TerrainRenderStage> terrain_renderer,
                             const std::shared_ptr<world::WorldRenderStage> world_renderer) :
	terrain_renderer{terrain_renderer},
	world_renderer{world_renderer} {
}

std::shared_ptr<terrain::RenderEntity> RenderFactory::add_terrain_render_entity(const util::Vector2s chunk_size,
                                                                                const coord::tile_delta chunk_offset) {
	auto entity = std::make_shared<terrain::RenderEntity>();
	this->terrain_renderer->add_render_entity(entity, chunk_size, chunk_offset.to_phys2().to_scene2());

	return entity;
}

std::shared_ptr<world::RenderEntity> RenderFactory::add_world_render_entity() {
	auto entity = std::make_shared<world::RenderEntity>();
	this->world_renderer->add_render_entity(entity);

	return entity;
}

} // namespace openage::renderer
