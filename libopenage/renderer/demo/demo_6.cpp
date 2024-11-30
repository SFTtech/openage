// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "demo_6.h"

#include <QKeyEvent>

#include "curve/continuous.h"
#include "curve/segmented.h"
#include "renderer/camera/camera.h"
#include "renderer/camera/frustum_2d.h"
#include "renderer/camera/frustum_3d.h"
#include "renderer/demo/util.h"
#include "renderer/gui/integration/public/gui_application_with_logger.h"
#include "renderer/opengl/window.h"
#include "renderer/render_pass.h"
#include "renderer/renderer.h"
#include "renderer/resources/animation/angle_info.h"
#include "renderer/resources/animation/frame_info.h"
#include "renderer/resources/animation/layer_info.h"
#include "renderer/resources/parser/parse_sprite.h"
#include "renderer/resources/parser/parse_terrain.h"
#include "renderer/resources/shader_source.h"
#include "renderer/resources/texture_info.h"
#include "renderer/shader_program.h"
#include "renderer/texture.h"
#include "renderer/uniform_buffer.h"
#include "time/clock.h"
#include "util/path.h"
#include "util/vector.h"


namespace openage::renderer::tests {

void renderer_demo_6(const util::Path &path) {
	auto render_mgr = RenderManagerDemo6{path};

	// Create render objects
	auto renderables_2d = render_mgr.create_2d_obj();
	auto renderables_3d = render_mgr.create_3d_obj();
	auto renderables_frame = render_mgr.create_frame_obj();

	// Add objects to the render passes
	render_mgr.obj_2d_pass->add_renderables(std::move(renderables_2d));
	render_mgr.obj_3d_pass->add_renderables({renderables_3d});
	render_mgr.frame_pass->add_renderables(std::move(renderables_frame));

	// Move the camera with the WASD keys
	// This is where we will also update the frustum for the 2D objects
	render_mgr.window->add_key_callback([&](const QKeyEvent &ev) {
		if (ev.type() == QEvent::KeyPress) {
			auto key = ev.key();

			// move_frame moves the camera in the specified direction in the next drawn frame
			switch (key) {
			case Qt::Key_W: { // forward
				render_mgr.camera->move_rel(Eigen::Vector3f(-1.0f, 0.0f, -1.0f), 0.2f);
			} break;
			case Qt::Key_A: { // left
				render_mgr.camera->move_rel(Eigen::Vector3f(-1.0f, 0.0f, 1.0f), 0.1f);
			} break;
			case Qt::Key_S: { // back
				render_mgr.camera->move_rel(Eigen::Vector3f(1.0f, 0.0f, 1.0f), 0.2f);
			} break;
			case Qt::Key_D: { // right
				render_mgr.camera->move_rel(Eigen::Vector3f(1.0f, 0.0f, -1.0f), 0.1f);
			} break;
			default:
				break;
			}

			// Update the camera uniform buffer
			auto new_cam_unifs = render_mgr.camera->get_uniform_buffer()->new_uniform_input(
				"view",
				render_mgr.camera->get_view_matrix(),
				"proj",
				render_mgr.camera->get_projection_matrix());
			render_mgr.camera->get_uniform_buffer()->update_uniforms(new_cam_unifs);

			/* Generate the frustum for the 2D objects */

			// Copy the camera used for drawing
			//
			// In this demo, we will manipulate the frustum camera
			// to create a slightly smaller frustum for the 2D objects
			// to show the effect of frustum culling.
			//
			// In the real renderer, the normal camera would be used.
			auto frustum_camera = *render_mgr.camera;

			// Downsize the frustum to 70% of the camera size
			float frustum_factor = 0.7f;
			auto frustum_cam_size = util::Vector2s{render_mgr.camera->get_viewport_size()[0] * frustum_factor,
			                                       render_mgr.camera->get_viewport_size()[1] * frustum_factor};
			frustum_camera.resize(frustum_cam_size[0], frustum_cam_size[1]);

			// Get a 2D frustum object
			auto frustum_2d = frustum_camera.get_frustum_2d();
			frustum_2d.update(frustum_camera.get_viewport_size(),
			                  frustum_camera.get_view_matrix(),
			                  frustum_camera.get_projection_matrix(),
			                  frustum_camera.get_zoom());

			// Check if the 2D scene objects are in the frustum
			// and update the renderables in the 2D render pass
			auto renderables_2d = render_mgr.create_2d_obj();
			std::vector<renderer::Renderable> renderables_in_frustum{};
			for (size_t i = 0; i < render_mgr.obj_2d_positions.size(); ++i) {
				auto pos = render_mgr.obj_2d_positions.at(i);
				bool in_frustum = frustum_2d.in_frustum(pos.to_world_space(),
				                                        Eigen::Matrix4f::Identity(),
				                                        render_mgr.animation_2d_info.get_scalefactor(),
				                                        render_mgr.animation_2d_info.get_max_bounds());
				if (in_frustum) {
					// Only add objects that are in the frustum
					renderables_in_frustum.push_back(renderables_2d.at(i));
				}
			}

			// Clear the renderables in the 2D render pass
			// and add ONLY the renderables that are inside the frustum
			render_mgr.obj_2d_pass->clear_renderables();
			render_mgr.obj_2d_pass->add_renderables(std::move(renderables_in_frustum));
		}
	});

	// Draw everything
	render_mgr.run();
}

RenderManagerDemo6::RenderManagerDemo6(const util::Path &path) :
	path{path} {
	this->setup();
}

void RenderManagerDemo6::run() {
	while (not window->should_close()) {
		this->qtapp->process_events();

		// Draw everything
		renderer->render(this->obj_3d_pass);
		renderer->render(this->obj_2d_pass);
		renderer->render(this->frame_pass);
		renderer->render(this->display_pass);

		// Display final output on screen
		this->window->update();
	}
}

const std::vector<renderer::Renderable> RenderManagerDemo6::create_2d_obj() {
	std::vector<renderer::Renderable> renderables;
	for (size_t i = 0; i < this->obj_2d_positions.size(); ++i) {
		// Create renderable for 2D animation
		auto scale = this->animation_2d_info.get_scalefactor();
		auto angle = this->obj_2d_angles.at(i);
		auto frame_info = this->animation_2d_info.get_layer(0).get_angle(angle)->get_frame(0);
		auto tex_id = frame_info->get_texture_idx();
		auto subtex_id = frame_info->get_subtexture_idx();
		auto subtex = this->animation_2d_info.get_texture(tex_id)->get_subtex_info(subtex_id);
		auto subtex_size = subtex.get_size();
		Eigen::Vector2f subtex_size_vec{
			static_cast<float>(subtex_size[0]),
			static_cast<float>(subtex_size[1])};
		auto anchor_params = subtex.get_anchor_params();
		auto anchor_params_vec = Eigen::Vector2f{
			static_cast<float>(anchor_params[0]),
			static_cast<float>(anchor_params[1])};
		auto tile_params = subtex.get_subtex_coords();
		auto scene_pos = this->obj_2d_positions.at(i);
		auto animation_2d_unifs = this->obj_2d_shader->new_uniform_input(
			"obj_world_position",
			scene_pos.to_world_space(),
			"scale",
			scale,
			"subtex_size",
			subtex_size_vec,
			"anchor_offset",
			anchor_params_vec,
			"tex",
			this->obj_2d_texture,
			"tile_params",
			tile_params);
		auto quad = this->renderer->add_mesh_geometry(resources::MeshData::make_quad());
		Renderable animation_2d_obj{
			animation_2d_unifs,
			quad,
			true,
			true,
		};

		renderables.push_back(animation_2d_obj);
	}

	return renderables;
}

const renderer::Renderable RenderManagerDemo6::create_3d_obj() {
	// Create renderable for terrain
	auto terrain_unifs = this->obj_3d_shader->new_uniform_input(
		"tex",
		this->obj_3d_texture);

	std::vector<coord::scene3> terrain_pos{};
	terrain_pos.push_back({-25, -25, 0});
	terrain_pos.push_back({25, -25, 0});
	terrain_pos.push_back({-25, 25, 0});
	terrain_pos.push_back({25, 25, 0});
	std::vector<float> terrain_verts{};
	for (size_t i = 0; i < terrain_pos.size(); ++i) {
		auto scene_pos = terrain_pos.at(i).to_world_space();
		terrain_verts.push_back(scene_pos[0]);
		terrain_verts.push_back(scene_pos[1]);
		terrain_verts.push_back(scene_pos[2]);
		terrain_verts.push_back(0.0f + i / 2);
		terrain_verts.push_back(0.0f + i % 2);
	}
	auto vert_info = resources::VertexInputInfo{
		{resources::vertex_input_t::V3F32, resources::vertex_input_t::V2F32},
		resources::vertex_layout_t::AOS,
		resources::vertex_primitive_t::TRIANGLE_STRIP,
	};
	std::vector<uint8_t> vert_data(terrain_verts.size() * sizeof(float));
	std::memcpy(vert_data.data(), terrain_verts.data(), vert_data.size());
	auto terrain_mesh = resources::MeshData{std::move(vert_data), vert_info};
	auto terrain_geometry = this->renderer->add_mesh_geometry(terrain_mesh);
	Renderable terrain_obj{
		terrain_unifs,
		terrain_geometry,
		true,
		true,
	};

	return terrain_obj;
}

const std::vector<renderer::Renderable> RenderManagerDemo6::create_frame_obj() {
	std::vector<renderer::Renderable> renderables;
	for (auto scene_pos : this->obj_2d_positions) {
		// Create renderables for frame around sprites
		std::array<float, 8> frame_verts{-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f};
		std::vector<uint8_t> frame_vert_data(frame_verts.size() * sizeof(float));
		std::memcpy(frame_vert_data.data(), frame_verts.data(), frame_vert_data.size());
		auto frame_vert_info = resources::VertexInputInfo{
			{resources::vertex_input_t::V2F32},
			resources::vertex_layout_t::AOS,
			resources::vertex_primitive_t::LINE_LOOP,
		};
		auto frame_mesh = resources::MeshData{std::move(frame_vert_data), frame_vert_info};
		auto frame_geometry = this->renderer->add_mesh_geometry(frame_mesh);

		auto scale = this->animation_2d_info.get_scalefactor();
		auto max_frame_size = this->animation_2d_info.get_max_size();
		auto frame_size = Eigen::Vector2f{
			static_cast<float>(max_frame_size[0]),
			static_cast<float>(max_frame_size[1])};
		auto frame_unifs = this->frame_shader->new_uniform_input(
			"obj_world_position",
			scene_pos.to_world_space(),
			"scale",
			scale,
			"frame_size",
			frame_size,
			"incol",
			Eigen::Vector4f{0.0f, 0.0f, 1.0f, 1.0f});
		Renderable frame_obj{
			frame_unifs,
			frame_geometry,
			true,
			true,
		};

		renderables.push_back(frame_obj);
	}

	// Create renderable for frustum frame
	std::array<float, 8> frame_verts{-0.7f, -0.7f, -0.7f, 0.7f, 0.7f, 0.7f, 0.7f, -0.7f};
	std::vector<uint8_t> frame_vert_data(frame_verts.size() * sizeof(float));
	std::memcpy(frame_vert_data.data(), frame_verts.data(), frame_vert_data.size());
	auto frame_vert_info = resources::VertexInputInfo{
		{resources::vertex_input_t::V2F32},
		resources::vertex_layout_t::AOS,
		resources::vertex_primitive_t::LINE_LOOP,
	};
	auto frame_mesh = resources::MeshData{std::move(frame_vert_data), frame_vert_info};
	auto frame_geometry = this->renderer->add_mesh_geometry(frame_mesh);

	auto frame_unifs = this->frustum_shader->new_uniform_input(
		"incol",
		Eigen::Vector4f{1.0f, 0.0f, 0.0f, 1.0f});
	Renderable frame_obj{
		frame_unifs,
		frame_geometry,
		true,
		true,
	};
	renderables.push_back(frame_obj);

	return renderables;
}

void RenderManagerDemo6::setup() {
	this->qtapp = std::make_shared<gui::GuiApplicationWithLogger>();

	// Create the window and renderer
	window_settings settings;
	settings.width = 1024;
	settings.height = 768;
	settings.debug = true;
	this->window = std::make_shared<opengl::GlWindow>("openage renderer test", settings);
	this->renderer = window->make_renderer();

	this->load_shaders();
	this->load_assets();
	this->create_camera();
	this->create_render_passes();
}

void RenderManagerDemo6::load_shaders() {
	// Shader
	auto shaderdir = this->path / "assets" / "test" / "shaders";

	/* Shader for 3D objects*/
	auto obj_vshader_file = (shaderdir / "demo_6_3d.vert.glsl").open();
	auto obj_vshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		obj_vshader_file.read());
	obj_vshader_file.close();

	auto obj_fshader_file = (shaderdir / "demo_6_3d.frag.glsl").open();
	auto obj_fshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		obj_fshader_file.read());
	obj_fshader_file.close();

	/* Shader for 2D animations */
	auto obj_2d_vshader_file = (shaderdir / "demo_6_2d.vert.glsl").open();
	auto obj_2d_vshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		obj_2d_vshader_file.read());
	obj_2d_vshader_file.close();

	auto obj_2d_fshader_file = (shaderdir / "demo_6_2d.frag.glsl").open();
	auto obj_2d_fshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		obj_2d_fshader_file.read());
	obj_2d_fshader_file.close();

	/* Shader for frames */
	auto frame_vshader_file = (shaderdir / "demo_6_2d_frame.vert.glsl").open();
	auto frame_vshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		frame_vshader_file.read());
	frame_vshader_file.close();

	auto frame_fshader_file = (shaderdir / "demo_6_2d_frame.frag.glsl").open();
	auto frame_fshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		frame_fshader_file.read());
	frame_fshader_file.close();

	/* Shader for frustrum frame */
	auto frustum_vshader_file = (shaderdir / "demo_6_2d_frustum_frame.vert.glsl").open();
	auto frustum_vshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		frustum_vshader_file.read());
	frustum_vshader_file.close();

	// Use the same fragment shader as the frame shader
	auto frustum_fshader_src = frame_fshader_src;

	/* Shader for rendering to the screen */
	auto display_vshader_file = (shaderdir / "demo_6_display.vert.glsl").open();
	auto display_vshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		display_vshader_file.read());
	display_vshader_file.close();

	auto display_fshader_file = (shaderdir / "demo_6_display.frag.glsl").open();
	auto display_fshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		display_fshader_file.read());
	display_fshader_file.close();

	// Create shader programs
	this->obj_3d_shader = this->renderer->add_shader({obj_vshader_src, obj_fshader_src});
	this->obj_2d_shader = this->renderer->add_shader({obj_2d_vshader_src, obj_2d_fshader_src});
	this->frame_shader = this->renderer->add_shader({frame_vshader_src, frame_fshader_src});
	this->frustum_shader = this->renderer->add_shader({frustum_vshader_src, frustum_fshader_src});
	this->display_shader = this->renderer->add_shader({display_vshader_src, display_fshader_src});
}

void RenderManagerDemo6::load_assets() {
	// Load assets for 2D objects
	auto animation_2d_path = this->path / "assets" / "test" / "textures" / "test_tank.sprite";
	this->animation_2d_info = resources::parser::parse_sprite_file(animation_2d_path);

	auto tex_info = this->animation_2d_info.get_texture(0);
	auto tex_data = resources::Texture2dData{*tex_info};
	this->obj_2d_texture = this->renderer->add_texture(tex_data);

	// Load assets for 3D objects
	auto terrain_path = this->path / "assets" / "test" / "textures" / "test_terrain.terrain";
	this->terrain_3d_info = resources::parser::parse_terrain_file(terrain_path);

	auto terrain_tex_info = this->terrain_3d_info.get_texture(0);
	auto terrain_tex_data = resources::Texture2dData{*terrain_tex_info};
	this->obj_3d_texture = this->renderer->add_texture(terrain_tex_data);
}

void RenderManagerDemo6::create_camera() {
	// Camera
	this->camera = std::make_shared<renderer::camera::Camera>(renderer, window->get_size());
	this->camera->set_zoom(2.0f);

	// Bind the camera uniform buffer to the shaders
	obj_3d_shader->bind_uniform_buffer("camera", this->camera->get_uniform_buffer());
	obj_2d_shader->bind_uniform_buffer("camera", this->camera->get_uniform_buffer());
	frame_shader->bind_uniform_buffer("camera", this->camera->get_uniform_buffer());

	// Update the camera uniform buffer
	auto camera_unifs = camera->get_uniform_buffer()->new_uniform_input(
		"view",
		this->camera->get_view_matrix(),
		"proj",
		this->camera->get_projection_matrix(),
		"inv_zoom",
		1.0f / this->camera->get_zoom());
	auto viewport_size = this->camera->get_viewport_size();
	Eigen::Vector2f viewport_size_vec{
		1.0f / static_cast<float>(viewport_size[0]),
		1.0f / static_cast<float>(viewport_size[1])};
	camera_unifs->update("inv_viewport_size", viewport_size_vec);
	this->camera->get_uniform_buffer()->update_uniforms(camera_unifs);
}

void RenderManagerDemo6::create_render_passes() {
	// Create render passes
	auto window_size = window->get_size();
	auto color_texture0 = renderer->add_texture(resources::Texture2dInfo(window_size[0],
	                                                                     window_size[1],
	                                                                     resources::pixel_format::rgba8));
	auto fbo0 = renderer->create_texture_target({color_texture0});
	this->obj_3d_pass = renderer->add_render_pass({}, fbo0);

	auto color_texture1 = renderer->add_texture(resources::Texture2dInfo(window_size[0],
	                                                                     window_size[1],
	                                                                     resources::pixel_format::rgba8));
	auto fbo1 = renderer->create_texture_target({color_texture1});
	this->obj_2d_pass = renderer->add_render_pass({}, fbo1);

	auto color_texture2 = renderer->add_texture(resources::Texture2dInfo(window_size[0],
	                                                                     window_size[1],
	                                                                     resources::pixel_format::rgba8));
	auto fbo2 = renderer->create_texture_target({color_texture2});
	this->frame_pass = renderer->add_render_pass({}, fbo2);

	// Create render pass for rendering to screen
	auto quad = renderer->add_mesh_geometry(resources::MeshData::make_quad());
	auto color_texture0_unif = display_shader->new_uniform_input("color_texture", color_texture0);
	Renderable display_obj_3d{
		color_texture0_unif,
		quad,
		true,
		true,
	};
	auto color_texture1_unif = display_shader->new_uniform_input("color_texture", color_texture1);
	Renderable display_obj_2d{
		color_texture1_unif,
		quad,
		true,
		true,
	};
	auto color_texture2_unif = display_shader->new_uniform_input("color_texture", color_texture2);
	Renderable display_obj_frame{
		color_texture2_unif,
		quad,
		true,
		true,
	};
	this->display_pass = renderer->add_render_pass(
		{display_obj_3d, display_obj_2d, display_obj_frame},
		renderer->get_display_target());

	if (not check_uniform_completeness({display_obj_3d, display_obj_2d, display_obj_frame})) {
		log::log(WARN << "Uniforms not complete.");
	}
}

} // namespace openage::renderer::tests
