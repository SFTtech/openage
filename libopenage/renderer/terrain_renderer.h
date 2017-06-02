// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <unordered_map>

#include <eigen3/Eigen/Core>

namespace openage {
namespace renderer {

	struct Tile {
		using terrain_id_t = size_t;

		terrain_id_t terrain_id;
		size_t subtexture_id;
		Eigen::Vector3f position; //y is up, z is depth
	};

	class GameRenderer;


	class TerrainRenderer {
		public:
			TerrainRenderer(GameRenderer& renderer);

			void render(const Eigen::Matrix4f& view_project_transform) const;

			void add_tiles(const std::vector<Tile>& tiles);
			void remove_tiles(const std::vector<Tile>& tiles);

			void add_terrain_texture(const Tile::terrain_id_t id, const std::shared_ptr<Texture> texturePtr);

		private:
			struct StoredTile {
				Tile tile;
				std::unique_ptr<UniformInput> uniform_input_ptr;
				Renderable renderable;
				Eigen::Matrix4f model_transform;
			};

			static const Eigen::Matrix4f TILE_SCALE_TRANSFORM;
			static const std::string MVP_UNIFORM_NAME;
			static const std::string TEXTURE_ATLAS_UNIFORM_NAME;
			static const std::string TEXCOORD_TRANSFORM_UNIFORM_NAME;

			GameRenderer& game_renderer;

			std::vector<StoredTile> tiles; //can be changed to better datastructure (octree, kd-tree, bsp-tree, ...) later
			std::unordered_map<Tile::terrain_id_t, std::shared_ptr<Texture>> terrain_texture_map;

            std::unique_ptr<ShaderProgram> terrain_shader_ptr;
			RenderPass render_pass;

            static Eigen::Matrix4f calculateTileScaleTransform();
            Eigen::Matrix3f calculate_texcoord_transform(const Tile& tile);

            void remove_tile(const Tile& tile);
	};

}}
