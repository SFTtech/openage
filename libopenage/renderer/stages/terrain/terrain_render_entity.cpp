// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#include "terrain_render_entity.h"

#include "renderer/renderer.h"
#include "renderer/resources/texture_data.h"
#include "renderer/texture.h"

namespace openage::renderer::terrain {

TerrainRenderEntity::TerrainRenderEntity(const std::shared_ptr<renderer::Renderer> &renderer,
                                         const util::Path &assetdir) :
	size{0, 0},
	vertices{},
	texture{},
	renderer{renderer},
	assetdir{assetdir} {
}

void TerrainRenderEntity::update() {
	// TODO: Update from gamestate
}

const std::vector<TerrainVertex> &TerrainRenderEntity::get_vertices() {
	return this->vertices;
}

const std::shared_ptr<renderer::Texture2d> &TerrainRenderEntity::get_textures() {
	// ASDF: testing
	auto test_path = this->assetdir / "textures" / "test_terrain_tex.png";
	const auto tex_data = renderer::resources::Texture2dData(test_path);
	this->texture = this->renderer->add_texture(tex_data);

	return this->texture;
}

const util::Vector2s &TerrainRenderEntity::get_size() {
	return this->size;
}

} // namespace openage::renderer::terrain