// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "coord/scene.h"
#include "renderer/renderable.h"
#include "renderer/resources/animation/animation_info.h"
#include "renderer/resources/terrain/terrain_info.h"
#include "util/path.h"


namespace openage::renderer {
class RenderPass;
class Renderer;
class ShaderProgram;
class Texture2d;

namespace camera {
class Camera;
}

namespace gui {
class GuiApplicationWithLogger;
}

namespace opengl {
class GlWindow;
}

namespace tests {

/**
 * Show the usage of frustum culling in the renderer.
 *     - Window creation
 *     - Loading shaders
 *     - Creating a camera
 *     - 2D and 3D frustum retrieval
 *     - Manipulating the frustum
 *     - Rendering objects with frustum culling
 *
 * @param path Path to the openage asset directory.
 */
void renderer_demo_6(const util::Path &path);


/**
 * Render manager that handles drawing, object creation, etc.
 */
class RenderManagerDemo6 {
public:
	RenderManagerDemo6(const util::Path &path);
	void run();

	/* Create objects to render */

	/// Create 2D objects (sprites)
	const std::vector<renderer::Renderable> create_2d_obj();
	/// Create 3D objects (terrain)
	const renderer::Renderable create_3d_obj();
	/// Create frames around 2D objects. These represents the boundaries of the objects
	/// that are used by the frustum culling algorithm.
	const std::vector<renderer::Renderable> create_frame_obj();

	/// Qt application
	std::shared_ptr<gui::GuiApplicationWithLogger> qtapp;

	/// OpenGL window
	std::shared_ptr<opengl::GlWindow> window;

	/// Camera
	std::shared_ptr<camera::Camera> camera;

	/// Render passes
	std::shared_ptr<renderer::RenderPass> obj_2d_pass;
	std::shared_ptr<renderer::RenderPass> obj_3d_pass;
	std::shared_ptr<renderer::RenderPass> frame_pass;
	std::shared_ptr<renderer::RenderPass> display_pass;

	/// 2D object (sprite) positions
	const std::array<coord::scene3, 5> obj_2d_positions = {
		coord::scene3{0, 0, 0},
		coord::scene3{-4, -4, 0},
		coord::scene3{4, 4, 0},
		coord::scene3{-2, 3, 0},
		coord::scene3{3, -2, 0},
	};

	/// Rendered angles of the 2D objects
	const std::array<size_t, 5> obj_2d_angles = {0, 1, 2, 3, 4};

	/// Animation and texture information
	resources::Animation2dInfo animation_2d_info;
	resources::TerrainInfo terrain_3d_info;

private:
	/// Setup everything necessary for rendering.
	void setup();

	/// Load shaders, assets, create camera, and render passes.
	void load_shaders();
	void load_assets();
	void create_camera();
	void create_render_passes();

	/// Directory path
	util::Path path;

	/// Renderer
	std::shared_ptr<renderer::Renderer> renderer;

	/// Shaders
	std::shared_ptr<renderer::ShaderProgram> obj_2d_shader;
	std::shared_ptr<renderer::ShaderProgram> obj_3d_shader;
	std::shared_ptr<renderer::ShaderProgram> frame_shader;
	std::shared_ptr<renderer::ShaderProgram> frustum_shader;
	std::shared_ptr<renderer::ShaderProgram> display_shader;

	/// Textures for the rendered objects (2D and 3D)
	std::shared_ptr<renderer::Texture2d> obj_2d_texture;
	std::shared_ptr<renderer::Texture2d> obj_3d_texture;
};

} // namespace tests
} // namespace openage::renderer
