// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#include "demo_5.h"

#include <eigen3/Eigen/Dense>
#include <epoxy/gl.h>
#include <QMouseEvent>

#include "renderer/camera/camera.h"
#include "renderer/demo/util.h"
#include "renderer/gui/integration/public/gui_application_with_logger.h"
#include "renderer/opengl/window.h"
#include "renderer/render_pass.h"
#include "renderer/render_target.h"
#include "renderer/resources/buffer_info.h"
#include "renderer/resources/shader_source.h"
#include "renderer/resources/texture_data.h"
#include "renderer/shader_program.h"
#include "renderer/uniform_buffer.h"
#include "renderer/uniform_input.h"


namespace openage::renderer::tests {

void renderer_demo_5(const util::Path &path) {
	auto qtapp = std::make_shared<gui::GuiApplicationWithLogger>();

	window_settings settings;
	settings.width = 800;
	settings.height = 600;
	settings.debug = true;
	opengl::GlWindow window("openage renderer test", settings);
	auto renderer = window.make_renderer();
	auto size = window.get_size();

	// Camera setup
	auto camera = std::make_shared<renderer::camera::Camera>(renderer, size);
	window.add_resize_callback([&](size_t w, size_t h, double /*scale*/) {
		camera->resize(w, h);
	});

	/* Display the subtextures using the meta information */
	log::log(INFO << "Loading shaders...");

	auto shaderdir = path / "assets" / "test" / "shaders";

	/* Shader for individual objects in pass 1. */
	auto obj_vshader_file = (shaderdir / "demo_5_obj.vert.glsl").open();
	auto obj_vshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		obj_vshader_file.read());
	obj_vshader_file.close();

	auto obj_fshader_file = (shaderdir / "demo_5_obj.frag.glsl").open();
	auto obj_fshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		obj_fshader_file.read());
	obj_fshader_file.close();

	auto obj_shader = renderer->add_shader({obj_vshader_src, obj_fshader_src});

	// object texture
	auto tex = resources::Texture2dData(path / "/assets/test/textures/test_terrain.png");
	auto gltex = renderer->add_texture(tex);

	// object vertices
	// in the scene this is just a very large square that gets rendered like
	// a diamond terrain shape
	// clang-format off
	std::vector<float> verts{
		0.0f,   0.0f,   0.0f, -1.0f, -1.0f, // bottom left
		-10.0f, 0.0f,   0.0f, -1.0f,  1.0f, // bottom right
		0.0f,   0.0f, -10.0f,  1.0f, -1.0f, // top left
		-10.0f, 0.0f, -10.0f,  1.0f,  1.0f, // top right
	};
	// clang-format on

	// create the mesh info
	resources::VertexInputInfo info{
		{resources::vertex_input_t::V3F32, resources::vertex_input_t::V2F32},
		resources::vertex_layout_t::AOS,
		resources::vertex_primitive_t::TRIANGLE_STRIP};

	auto const vert_data_size = verts.size() * sizeof(float);
	std::vector<uint8_t> vert_data(vert_data_size);
	std::memcpy(vert_data.data(), verts.data(), vert_data_size);

	resources::MeshData meshdata{std::move(vert_data), info};

	// create a geometry object from the mesh data
	auto geometry = renderer->add_mesh_geometry(meshdata);

	// create a uniform storage for the shader
	auto transform_unifs = obj_shader->create_empty_input();

	// create the renderable object from the geometry and the uniforms
	Renderable terrain_obj{
		transform_unifs,
		geometry,
	};

	// create the render pass
	// this directly renders outputs the rendered object to the screen
	auto pass = renderer->add_render_pass({terrain_obj}, renderer->get_display_target());

	// create the uniform buffer for the camera matrices
	// and bind it to the shader
	// this sets the binding point of the shader to the bindng point of the buffer
	resources::UBOInput view_input{"view", resources::ubo_input_t::M4F32};
	resources::UBOInput proj_input{"proj", resources::ubo_input_t::M4F32};
	auto ubo_info = resources::UniformBufferInfo{resources::ubo_layout_t::STD140, {view_input, proj_input}};
	auto uniform_buffer = renderer->add_uniform_buffer(ubo_info);
	obj_shader->bind_uniform_buffer("cam", uniform_buffer);

	// create a uniform buffer storage
	// pass the camera view and projection matrices to the buffer storage
	// this does not update the buffer yet, it only prepares the data before the upload
	auto unif_in = uniform_buffer->new_uniform_input(
		"view",
		camera->get_view_matrix(),
		"proj",
		camera->get_projection_matrix());

	// pass the uniform storage to the buffer object
	// this uploads the updates to the buffer on the GPU
	uniform_buffer->update_uniforms(unif_in);

	// set the non-buffer uniforma for the object
	Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
	transform_unifs->update(
		"model",
		model,
		"tex",
		gltex);

	if (not check_uniform_completeness({terrain_obj})) {
		log::log(WARN << "Uniforms not complete.");
	}

	// Move around the scene with WASD
	window.add_key_callback([&](const QKeyEvent &ev) {
		bool cam_update = false;
		if (ev.type() == QEvent::KeyPress) {
			auto key = ev.key();

			switch (key) {
			case Qt::Key_W: { // forward
				camera->move_rel(Eigen::Vector3f(-1.0f, 0.0f, -1.0f), 0.5f);
				cam_update = true;

				log::log(INFO << "Camera moved forward.");
			} break;
			case Qt::Key_A: { // left
				// half the speed because the relationship between forward/back and
				// left/right is 1:2 in our ortho projection.
				camera->move_rel(Eigen::Vector3f(-1.0f, 0.0f, 1.0f), 0.25f);
				cam_update = true;

				log::log(INFO << "Camera moved left.");
			} break;
			case Qt::Key_S: { // back
				camera->move_rel(Eigen::Vector3f(1.0f, 0.0f, 1.0f), 0.5f);
				cam_update = true;

				log::log(INFO << "Camera moved back.");
			} break;
			case Qt::Key_D: { // right
				// half the speed because the relationship between forward/back and
				// left/right is 1:2 in our ortho projection.
				camera->move_rel(Eigen::Vector3f(1.0f, 0.0f, -1.0f), 0.25f);
				cam_update = true;

				log::log(INFO << "Camera moved right.");
			} break;
			default:
				break;
			}
		}

		if (cam_update) {
			// update the uniform buffer only if the camera has moved
			unif_in->update("view", camera->get_view_matrix());
			uniform_buffer->update_uniforms(unif_in);

			log::log(INFO << "Uniform buffer updated.");
		}
	});

	while (not window.should_close()) {
		qtapp->process_events();

		renderer->render(pass);
		window.update();

		renderer->check_error();
	}
}

} // namespace openage::renderer::tests
