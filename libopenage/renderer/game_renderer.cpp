// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "game_renderer.h"

namespace openage {
namespace renderer {

    GameRenderer::GameRenderer(const size_t window_width, const size_t window_height)
        : window_width{window_width}, window_height{window_height}, terrain_renderer{*this}, unit_quad{geometry_t::quad}
    {
        update_camera_aspect_ratio();
    }

    void GameRenderer::change_window_size(const size_t width, const size_t height)
    {
        assert(width > 0 && height > 0);
        this->window_width = width;
        this->window_height = height;
        update_camera_aspect_ratio();
    }

    void GameRenderer::render() const
    {
        auto view_project_transform = this->camera.view_project_transform_2d();
        render_terrain(view_project_transform);
    }

    void GameRenderer::add_tiles(const std::vector<Tile>& tiles)
    {
        this->terrain_renderer.add_tiles(tiles);
    }

    void GameRenderer::remove_tiles(const std::vector<Tile>& tiles)
    {
        this->terrain_renderer.remove_tiles(tiles);
    }

    void GameRenderer::add_terrain_texture(const Tile::terrain_id_t id, const std::shared_ptr<Texture>& texturePtr)
    {
        this->terrain_renderer.add_terrain_texture(id, texturePtr);
    }

    void GameRenderer::move_camera(const Eigen::Vector3f &translate_vector)
    {
        this->camera.translate(translate_vector);
    }

    void GameRenderer::render_terrain(const Eigen::Matrix4f &view_project_transform) const
    {
        this->terrain_renderer.render(view_project_transform);
    }

    void GameRenderer::update_camera_aspect_ratio()
    {
        this->camera.aspect_ratio(static_cast<float>(this->window_width) / this->window_height);
    }

    Geometry* GameRenderer::unit_quad_ptr()
    {
        return &(this->unit_quad);
    }

}}
