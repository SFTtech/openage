// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <vector>

#include "renderer/resources/mesh_data.h"
#include <eigen3/Eigen/Dense>

namespace openage::renderer {
class Texture2d;

namespace terrain {
class TerrainRenderEntity;

class TerrainRenderMesh {
public:
	/**
     * Create a new terrain render mesh.
     *
     * @param entity Terrain render entity for updating vertex coordinates.
     */
	TerrainRenderMesh(const std::shared_ptr<TerrainRenderEntity> &entity);
	~TerrainRenderMesh() = default;

	/**
     * Set the terrain render entity for vertex updates of this mesh.
     *
     * @param entity New terrain render entity.
     */
	void set_render_entity(const std::shared_ptr<TerrainRenderEntity> &entity);

	/**
     * Recalculate the vertex positions for this mesh with information
     * from the currently set terrain render entity.
     */
	void update();

	/**
     * Get the vertex mesh for the terrain.
     *
     * @return Mesh for creating a renderer geometry object.
     */
	const renderer::resources::MeshData &get_mesh();

	/**
      * Get the texture that should be drawn onto the mesh.
      *
      * @return Texture object.
      */
	const std::shared_ptr<renderer::Texture2d> &get_texture();

private:
	/**
     * Source for ingame terrain coordinates. These coordinates are translated into
     * our render vertex mesh when \p update() is called.
     */
	std::shared_ptr<TerrainRenderEntity> render_entity;

     /**
      * Texture used for the mesh.
      */
     std::shared_ptr<renderer::Texture2d> texture;

	/**
     * Pre-transformation vertices for the terrain model.
     */
	renderer::resources::MeshData mesh;
};
} // namespace terrain
} // namespace openage::renderer