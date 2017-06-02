// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "terrain_renderer.h"
#include "game_renderer.h"

#include "../log/log.h"

#include <eigen3/Eigen/Dense>

namespace openage {
namespace renderer {

    const Eigen::Matrix4f TerrainRenderer::TILE_SCALE_TRANSFORM{calculateTileScaleTransform()};
    const std::string TerrainRenderer::MVP_UNIFORM_NAME{"u_model_view_project"};
    const std::string TerrainRenderer::TEXTURE_ATLAS_UNIFORM_NAME{"u_texture_atlas"};
    const std::string TerrainRenderer::TEXCOORD_TRANSFORM_UNIFORM_NAME{"u_texcoord_transform"};

    TerrainRenderer::TerrainRenderer(GameRenderer& renderer)
		: game_renderer(renderer)
	{
        this->render_pass.target = this->game_renderer.renderer().get_display_target();
	}

    Eigen::Matrix4f TerrainRenderer::calculateTileScaleTransform()
    {
        static const Eigen::Vector3f TILE_SCALE_VECTOR(1.0f, 0.5, 1.0f);
        auto scaleTransform = Eigen::Affine3f::Identity();
        scaleTransform.prescale(TILE_SCALE_VECTOR);
        return scaleTransform.matrix();
    }

	void TerrainRenderer::render(const Eigen::Matrix4f& view_project_transform) const
	{
		for (const auto& tile : tiles) {
			auto mvp_matrix = view_project_transform * tile.model_transform;
            this->terrain_shader_ptr->update_uniform_input(tile.renderable.unif_in, MVP_UNIFORM_NAME, mvp_matrix);
		}
		game_renderer->renderer().render(render_pass);
	}

	void TerrainRenderer::add_tiles(const std::vector<Tile>& tiles)
	{
		for (const auto& tile : tiles) {
			tiles.emplace_back();
			StoredTile& internal_tile = tiles.back();
			
			auto translate_transform = Eigen::Affine3f::Identity();
			translate_transform.pretranslate(tile.position);
			internal_tile.model_transform = translate_transform.matrix() * TILE_SCALE_TRANSFORM;

            internal_tile.uniform_input_ptr = this->terrain_shader_ptr->new_uniform_input(
					MVP_UNIFORM_NAME, internal_tile.model_transform,
					TEXTURE_ATLAS_UNIFORM_NAME, terrain_texture_map[tile.terrain_id].get(),
					TEXCOORD_TRANSFORM_UNIFORM_NAME, calculate_texcoord_transform(tile)
			);
			auto unit_quad_ptr = game_renderer->unit_quad_ptr();
			internal_tile.renderable = Renderable{
				internal_tile.uniform_input_ptr.get(),
				unit_quad_ptr,
				false,
				false
			};

            this->render_pass.renderables.push_back(internal_tile.renderable);
		}

	}

    Eigen::Matrix3f TerrainRenderer::calculate_texcoord_transform(const Tile& tile)
    {
        const auto& texture_atlas = *(this->terrain_texture_map.at(tile.terrain_id));
        const auto texture_atlas_size = texture_atlas.get_size();
        const auto& subtexture = texture_atlas.get_subtexture(tile.subtexture_id);

        const float position_x_normalized = static_cast<float>(subtexture.x) / texture_atlas_size.first;
        const float position_y_normalized = static_cast<float>(subtexture.y) / texture_atlas_size.second;

        const float scale_x = static_cast<float>(subtexture.w) / texture_atlas_size.first;
        const float scale_y = static_cast<float>(subtexture.h) / texture_atlas_size.second;

        auto transform = Eigen::Affine2f::Identity();
        transform.prescale(Eigen::Vector2f(scale_x, scale_y));
        transform.pretranslate(Eigen::Vector2f(position_x_normalized, position_y_normalized));
        return transform.matrix();
    }

    void TerrainRenderer::remove_tiles(const std::vector<Tile>& tiles)
    {
        for (const auto& tile : tiles) {
            remove_tile(tile);
        }
    }

    void TerrainRenderer::remove_tile(const Tile& tile)
    {
        //remove the corresponding internal tile for the given tile and the renderables from the renderpass
        auto iter_tiles = this->tiles.begin();
        auto iter_renderables = this->render_pass.renderables.begin();

        while (iter_tiles != this->tiles.end() && iter_renderables != this->render_pass.renderables.end()) {
            //remove if the position of the given tile matches the position of the current element.
            if (iter_tiles->model_tranform.block<1, 3>(3, 0) == tile.position) {
                iter_tiles = this->tiles.erase(iter_tiles);
                iter_renderables = this->renderpass.renderables.erase(iter_renderables);
            } else {
                ++iter_tiles;
                ++iter_renderables;
            }
        }
        assert(iter_tiles == this->tiles.end());
        assert(iter_drawables == this->render_pass.renderables.end());
    }

    void TerrainRenderer::add_terrain_texture(const Tile::terrain_id_t id, const std::shared_ptr<Texture> texturePtr)
    {
        if (this->terrain_texture_map.count(id) == 1) {
            log::log(WARN << "Replacing terrain texture atlas for terrain id: " << id);
        }
        this->terrain_texture_map[id] = texturePtr;
    }

}}
