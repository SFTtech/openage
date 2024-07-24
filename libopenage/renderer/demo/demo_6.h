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


class RenderManagerDemo6 {
public:
	RenderManagerDemo6(const util::Path &path);
	void run();

	const std::vector<renderer::Renderable> create_2d_obj();
	const renderer::Renderable create_3d_obj();
	const std::vector<renderer::Renderable> create_frame_obj();

	std::shared_ptr<gui::GuiApplicationWithLogger> qtapp;

	std::shared_ptr<opengl::GlWindow> window;

	std::shared_ptr<camera::Camera> camera;

	std::shared_ptr<renderer::RenderPass> obj_2d_pass;
	std::shared_ptr<renderer::RenderPass> obj_3d_pass;
	std::shared_ptr<renderer::RenderPass> frame_pass;
	std::shared_ptr<renderer::RenderPass> display_pass;

	const std::array<coord::scene3, 5> obj_2d_positions = {
		coord::scene3{0, 0, 0},
		coord::scene3{-4, -4, 0},
		coord::scene3{4, 4, 0},
		coord::scene3{-2, 3, 0},
		coord::scene3{3, -2, 0},
	};

	resources::Animation2dInfo animation_2d_info;
	resources::TerrainInfo terrain_3d_info;

private:
	void setup();

	void load_shaders();
	void load_assets();
	void create_camera();
	void create_render_passes();

	util::Path path;

	std::shared_ptr<renderer::Renderer> renderer;

	std::shared_ptr<renderer::ShaderProgram> obj_2d_shader;
	std::shared_ptr<renderer::ShaderProgram> obj_3d_shader;
	std::shared_ptr<renderer::ShaderProgram> frame_shader;
	std::shared_ptr<renderer::ShaderProgram> display_shader;

	std::shared_ptr<renderer::Texture2d> obj_2d_texture;
	std::shared_ptr<renderer::Texture2d> obj_3d_texture;
};

} // namespace tests
} // namespace openage::renderer
