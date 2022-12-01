// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "renderer/resources/mesh_data.h"
#include "util/vector.h"

namespace openage::renderer {
class Renderer;
class Texture2d;
class UniformInput;

namespace resources {
class TextureManager;
}

namespace world {
class WorldRenderEntity;

class WorldObject {
public:
	WorldObject();
	~WorldObject() = default;

	/**
     * Set the world render entity.
     *
     * @param entity New world render entity.
     */
	void set_render_entity(const std::shared_ptr<WorldRenderEntity> &entity);

	/**
     * Recalculate the vertex positions for this mesh with information
     * from the currently set render entity.
     */
	void update();

	/**
     * Get the quad for creating the geometry.
     *
     * @return Mesh for creating a renderer geometry object.
     */
	const renderer::resources::MeshData get_mesh();

	/**
      * Get the texture that should be drawn onto the mesh.
      *
      * @return Texture object.
      */
	const std::shared_ptr<renderer::Texture2d> &get_texture();

	/**
      * Check whether a new renderable needs to be created for this mesh.
      *
      * If true, the old renderable should be removed from the render pass.
      * The updated uniforms and geometry should be passed to this mesh.
      * Afterwards, clear the requirement flag with \p clear_requires_renderable().
      *
      * @return true if a new renderable is required, else false.
      */
	bool requires_renderable();

	/**
      * Indicate to this mesh that a new renderable has been created.
      */
	void clear_requires_renderable();

	/**
	 * Check whether the object was changed by \p update().
	 *
	 * @return true if changes were made, else false.
	 */
	bool is_changed();

	/**
	 * Clear the update flag by setting it to false.
	 */
	void clear_changed_flag();

	/**
      * Set the reference to the uniform inputs of the renderable
      * associated with this object. Relevant uniforms are updated
      * when calling \p update().
      *
      * @param uniforms Uniform inputs of this object's renderable.
      */
	void set_uniforms(const std::shared_ptr<renderer::UniformInput> &uniforms);

private:
	/**
      * Stores whether a new renderable for this object needs to be created
      * for the render pass.
      */
	bool require_renderable;

	/**
	 * Stores whether the \p update() call changed the object.
	 */
	bool changed;

	/**
	 * Texture manager for central accessing and loading textures.
	 *
       * TODO: Replace with asset manager
       */
	std::shared_ptr<renderer::resources::TextureManager> texture_manager;

	/**
       * Source for positional and texture data.
       */
	std::shared_ptr<WorldRenderEntity> render_entity;

	/**
        * Texture used for the mesh.
        */
	util::Vector3f position;

	/**
        * Texture used for the mesh.
        */
	std::shared_ptr<renderer::Texture2d> texture;

	/**
        * Shader uniforms for the renderable in the terrain render pass.
        */
	std::shared_ptr<renderer::UniformInput> uniforms;
};
} // namespace world
} // namespace openage::renderer