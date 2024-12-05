// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#include "demo_1.h"

#include <eigen3/Eigen/Dense>
#include <epoxy/gl.h>
#include <QMouseEvent>

#include "renderer/demo/util.h"
#include "renderer/gui/integration/public/gui_application_with_logger.h"
#include "renderer/opengl/window.h"
#include "renderer/render_pass.h"
#include "renderer/render_target.h"
#include "renderer/resources/shader_source.h"
#include "renderer/resources/texture_data.h"
#include "renderer/shader_program.h"
#include "renderer/texture.h"
#include "util/math_constants.h"


namespace openage::renderer::tests {

void renderer_demo_1(const util::Path &path) {
	auto qtapp = std::make_shared<gui::GuiApplicationWithLogger>();

	window_settings settings;
	settings.width = 800;
	settings.height = 600;
	settings.debug = true;
	opengl::GlWindow window("openage renderer test", settings);
	auto renderer = window.make_renderer();

	auto shaderdir = path / "assets" / "test" / "shaders";

	/* Shader for individual objects in pass 1. */
	auto obj_vshader_file = (shaderdir / "demo_1_obj.vert.glsl").open();
	auto obj_vshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		obj_vshader_file.read());
	obj_vshader_file.close();

	auto obj_fshader_file = (shaderdir / "demo_1_obj.frag.glsl").open();
	auto obj_fshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		obj_fshader_file.read());
	obj_fshader_file.close();

	/* Shader for copying the framebuffer in pass 2. */
	auto display_vshader_file = (shaderdir / "demo_1_display.vert.glsl").open();
	auto display_vshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		display_vshader_file.read());
	display_vshader_file.close();

	auto display_fshader_file = (shaderdir / "demo_1_display.frag.glsl").open();
	auto display_fshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		display_fshader_file.read());
	display_fshader_file.close();

	auto obj_shader = renderer->add_shader({obj_vshader_src, obj_fshader_src});
	auto display_shader = renderer->add_shader({display_vshader_src, display_fshader_src});

	/* Texture for the clickable objects. */
	auto tex = resources::Texture2dData(path / "/assets/test/textures/gaben.png");
	auto gltex = renderer->add_texture(tex);

	auto transform1 = Eigen::Affine3f::Identity();
	transform1.prescale(Eigen::Vector3f(0.4f, 0.2f, 1.0f));
	transform1.prerotate(Eigen::AngleAxisf(30.0f * math::PI / 180.0f, Eigen::Vector3f::UnitX()));
	transform1.pretranslate(Eigen::Vector3f(-0.4f, -0.6f, 0.0f));

	/* Clickable objects on the screen consist of a transform (matrix which places each object
	 * in the correct location), an identifier and a reference to the texture. */
	auto obj1_unifs = obj_shader->new_uniform_input(
		"mv",
		transform1.matrix(),
		"u_id",
		1u,
		"tex",
		gltex);

	auto transform2 = Eigen::Affine3f::Identity();
	transform2.prescale(Eigen::Vector3f(0.3f, 0.1f, 1.0f));
	transform2.prerotate(Eigen::AngleAxisf(50.0f * math::PI / 180.0f, Eigen::Vector3f::UnitZ()));

	auto transform3 = transform2;

	transform2.pretranslate(Eigen::Vector3f(0.3f, 0.1f, 0.3f));

	auto obj2_unifs = obj_shader->new_uniform_input(
		"mv",
		transform2.matrix(),
		"u_id",
		2u,
		// TODO bug: this tex input spills over to all the other uniform inputs!
		"tex",
		gltex);

	transform3.prerotate(Eigen::AngleAxisf(90.0f * math::PI / 180.0f, Eigen::Vector3f::UnitZ()));
	transform3.pretranslate(Eigen::Vector3f(0.3f, 0.1f, 0.5f));

	auto obj3_unifs = obj_shader->new_uniform_input(
		"mv",
		transform3.matrix(),
		"u_id",
		3u,
		"tex",
		gltex);

	/* The objects are using built-in quadrilateral geometry. */
	auto quad = renderer->add_mesh_geometry(resources::MeshData::make_quad());
	Renderable obj1{
		obj1_unifs,
		quad,
		true,
		true,
	};

	Renderable obj2{
		obj2_unifs,
		quad,
		true,
		true,
	};

	Renderable obj3{
		obj3_unifs,
		quad,
		true,
		true,
	};

	/* Make a framebuffer for the first render pass to draw into. The framebuffer consists of a color texture
	 * to be copied onto the back buffer in pass 2, as well as an id texture which will contain the object ids
	 * which we can later read in order to determine which object was clicked. The depth texture is required,
	 * but mostly irrelevant in this case. */
	auto size = window.get_size();
	auto color_texture = renderer->add_texture(resources::Texture2dInfo(size[0], size[1], resources::pixel_format::rgba8));
	auto id_texture = renderer->add_texture(resources::Texture2dInfo(size[0], size[1], resources::pixel_format::r32ui));
	auto depth_texture = renderer->add_texture(resources::Texture2dInfo(size[0], size[1], resources::pixel_format::depth24));
	auto fbo = renderer->create_texture_target({color_texture, id_texture, depth_texture});

	/* Make an object to update the projection matrix in pass 1 according to changes in the screen size.
	 * Because uniform values are preserved across objects using the same shader in a single render pass,
	 * it is sufficient to set it once at the beginning of the pass. To do this, we use an object with no
	 * geometry, which will set the uniform but not render anything. */
	auto proj_unif = obj_shader->new_uniform_input();
	Renderable proj_update{
		proj_unif,
		nullptr,
		false,
		false,
	};

	auto pass1 = renderer->add_render_pass(
		{proj_update, obj1, obj2, obj3},
		fbo);

	/* Make an object encompassing the entire screen for the second render pass. The object
	 * will be textured with the color output of pass 1, effectively copying its framebuffer. */
	auto color_texture_unif = display_shader->new_uniform_input("color_texture", color_texture);
	Renderable display_obj{
		color_texture_unif,
		quad,
		false,
		false,
	};

	auto pass2 = renderer->add_render_pass({display_obj}, renderer->get_display_target());

	if (not check_uniform_completeness({obj1, obj2, obj3, proj_update, display_obj})) {
		log::log(WARN << "Uniforms not complete.");
	}

	/* Data retrieved from the object index texture. */
	resources::Texture2dData id_texture_data = id_texture->into_data();
	bool texture_data_valid = false;

	// TODO(Vtec234): move these into the renderer
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0, 1.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	window.add_mouse_button_callback([&](const QMouseEvent &ev) {
		if (ev.type() == QEvent::MouseButtonRelease) {
			auto qpos = ev.position();
			ssize_t x = qpos.x();
			ssize_t y = qpos.y();

			log::log(INFO << "Clicked at location (" << x << ", " << y << ")");
			if (not texture_data_valid) {
				id_texture_data = id_texture->into_data();
				texture_data_valid = true;
			}
			auto id = id_texture_data.read_pixel<uint32_t>(x, y);
			log::log(INFO << "Id-texture-value at location: " << id);
			if (id == 0) {
				log::log(INFO << "Clicked at non existent object");
			}
			else {
				log::log(INFO << "Object number " << id << " clicked.");
			}
		}
	});

	window.add_resize_callback([&](size_t w, size_t h, double /*scale*/) {
		/* Calculate a projection matrix for the new screen size. */
		float aspectRatio = float(w) / float(h);
		float xScale = 1.0 / aspectRatio;

		Eigen::Matrix4f pmat;
		pmat << xScale, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1;

		proj_unif->update("proj", pmat);

		/* Create a new FBO with the right dimensions. */
		color_texture = renderer->add_texture(resources::Texture2dInfo(w, h, resources::pixel_format::rgba8));
		id_texture = renderer->add_texture(resources::Texture2dInfo(w, h, resources::pixel_format::r32ui));
		depth_texture = renderer->add_texture(resources::Texture2dInfo(w, h, resources::pixel_format::depth24));
		fbo = renderer->create_texture_target({color_texture, id_texture, depth_texture});

		color_texture_unif->update("color_texture", color_texture);

		texture_data_valid = false;
		pass1->set_target(fbo);
	});

	while (not window.should_close()) {
		renderer->render(pass1);
		renderer->render(pass2);
		window.update();
		qtapp->process_events();

		renderer->check_error();
	}
	window.close();
}
} // namespace openage::renderer::tests
