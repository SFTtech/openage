// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "renderer.h"
#include "terrain_renderer.h"
#include "camera.h"
#include "opengl/geometry.h"

#include <eigen3/Eigen/Core>

namespace openage {
namespace renderer {

    class Texture;

	class GameRenderer {
		public:
			GameRenderer(const size_t window_width, const size_t window_height);

			void change_window_size(const size_t width, const size_t height);

			void render() const;

            void add_tiles(const std::vector<Tile>& tiles);
			void remove_tiles(const std::vector<Tile>& tiles);

            void add_terrain_texture(const Tile::terrain_id_t id, const std::shared_ptr<Texture>& texturePtr);

            void move_camera(const Eigen::Vector3f &translate_vector);

            Geometry* unit_quad_ptr();

		private:
			Renderer renderer;
			TerrainRenderer terrain_renderer;
            Camera camera;

			size_t window_width;
			size_t window_height;

            opengl::GlGeometry unit_quad;

            void render_terrain(const Eigen::Matrix4f &view_project_transform) const;
            void update_camera_aspect_ratio();
	};

}}
