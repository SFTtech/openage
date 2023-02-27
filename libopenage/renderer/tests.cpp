// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "tests.h"

#include <cstdlib>
#include <eigen3/Eigen/Dense>
#include <epoxy/gl.h>
#include <functional>
#include <memory>
#include <QMouseEvent>
#include <unordered_map>

#include "error/error.h"
#include "event/clock.h"
#include "log/log.h"
#include "renderer/camera/camera.h"
#include "renderer/geometry.h"
#include "renderer/gui/integration/public/gui_application_with_logger.h"
#include "renderer/opengl/window.h"
#include "renderer/render_factory.h"
#include "renderer/resources/animation/angle_info.h"
#include "renderer/resources/animation/frame_info.h"
#include "renderer/resources/assets/asset_manager.h"
#include "renderer/resources/mesh_data.h"
#include "renderer/resources/parser/parse_sprite.h"
#include "renderer/resources/parser/parse_texture.h"
#include "renderer/resources/shader_source.h"
#include "renderer/resources/texture_data.h"
#include "renderer/shader_program.h"
#include "renderer/stages/screen/screen_renderer.h"
#include "renderer/stages/skybox/skybox_renderer.h"
#include "renderer/stages/terrain/terrain_render_entity.h"
#include "renderer/stages/terrain/terrain_renderer.h"
#include "renderer/stages/world/world_render_entity.h"
#include "renderer/stages/world/world_renderer.h"
#include "renderer/texture.h"
#include "util/math_constants.h"

namespace openage::renderer::tests {

// Macro for debugging OpenGL state.
#define DBG_GL(txt) \
	printf("before %s\n", txt); \
	opengl::GlContext::check_error(); \
	printf("after %s\n", txt);

/**
 * Shows the renderer's ability to create textured renderable objects and
 * allow basic interaction with them via mouse/key callbacks.
 *
 * @param path Path to project rootdir.
 */
void renderer_demo_0(const util::Path &path) {
	auto qtapp = std::make_shared<gui::GuiApplicationWithLogger>();

	opengl::GlWindow window("openage renderer test", 800, 600);
	auto renderer = window.make_renderer();

	auto shaderdir = path / "assets" / "test" / "shaders";

	/* Shader for individual objects in pass 1. */
	auto obj_vshader_file = (shaderdir / "demo_0_obj.vert.glsl").open();
	auto obj_vshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		obj_vshader_file.read());
	obj_vshader_file.close();

	auto obj_fshader_file = (shaderdir / "demo_0_obj.frag.glsl").open();
	auto obj_fshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		obj_fshader_file.read());
	obj_fshader_file.close();

	/* Shader for copying the framebuffer in pass 2. */
	auto display_vshader_file = (shaderdir / "demo_0_display.vert.glsl").open();
	auto display_vshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		display_vshader_file.read());
	display_vshader_file.close();

	auto display_fshader_file = (shaderdir / "demo_0_display.frag.glsl").open();
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

	/* Data retrieved from the object index texture. */
	resources::Texture2dData id_texture_data = id_texture->into_data();
	bool texture_data_valid = false;

	// TODO(Vtec234): move these into the renderer
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0, 1.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	window.add_mouse_button_callback([&](const QMouseEvent &ev) {
		auto qpos = ev.position();
		ssize_t x = qpos.x();
		ssize_t y = qpos.y();

		log::log(INFO << "Clicked at location (" << x << ", " << y << ")");
		if (!texture_data_valid) {
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
	});

	window.add_resize_callback([&](size_t w, size_t h) {
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

/**
 * Loads .sprite and .texture meta information files and displays
 * subtextures from the attached texture images.
 *
 * @param path Path to the openage asset directory.
 */
void renderer_demo_1(const util::Path &path) {
	auto qtapp = std::make_shared<gui::GuiApplicationWithLogger>();

	opengl::GlWindow window("openage renderer test", 800, 600);
	auto renderer = window.make_renderer();

	/* Load texture file standalone. */
	auto tex_path = path / "assets/test/textures/test_texture.texture";
	auto tex_info = renderer::resources::parser::parse_texture_file(tex_path);

	log::log(INFO << "Loaded texture " << tex_path.resolve_native_path());
	log::log(INFO << "  imagefile: " << tex_info.get_image_path().value().resolve_native_path());
	log::log(INFO << "  size:");
	log::log(INFO << "    width: " << tex_info.get_size().first);
	log::log(INFO << "    height: " << tex_info.get_size().second);
	log::log(INFO << "  pxformat:");
	log::log(INFO << "    data size: " << tex_info.get_data_size()
	              << " ("
	              << tex_info.get_size().first << " x "
	              << tex_info.get_size().second << " x "
	              << renderer::resources::pixel_size(tex_info.get_format())
	              << ")");
	log::log(INFO << "    cbits: "
	              << "(currently unused)");
	log::log(INFO << "  subtex count: " << tex_info.get_subtexture_count());

	/* Load animation file using the texture. */
	auto sprite_path = path / "assets/test/textures/test_animation.sprite";
	auto sprite_info = renderer::resources::parser::parse_sprite_file(sprite_path);

	log::log(INFO << "Loaded animation " << sprite_path.resolve_native_path());
	log::log(INFO << "  texture count: " << sprite_info.get_texture_count());
	for (size_t i = 0; i < sprite_info.get_texture_count(); ++i) {
		log::log(INFO << "    texture " << i << ": "
		              << sprite_info.get_texture(i)->get_image_path().value().resolve_native_path());
	}

	log::log(INFO << "  scalefactor: " << sprite_info.get_scalefactor());
	log::log(INFO << "  layer count: " << sprite_info.get_layer_count());
	for (size_t i = 0; i < sprite_info.get_layer_count(); ++i) {
		auto layer_info = sprite_info.get_layer(i);
		log::log(INFO << "    layer " << i << ":");
		log::log(INFO << "      position: " << layer_info.get_position());
		log::log(INFO << "      time per frame: " << layer_info.get_time_per_frame());
		log::log(INFO << "      replay delay: " << layer_info.get_replay_delay());
		log::log(INFO << "      angle count: " << layer_info.get_angle_count());

		for (size_t i = 0; i < layer_info.get_angle_count(); ++i) {
			auto angle_info = layer_info.get_angle(i);
			log::log(INFO << "      angle " << i << ":");
			log::log(INFO << "        degree: " << angle_info->get_angle_start());
			log::log(INFO << "        frame count: " << angle_info->get_frame_count());
			log::log(INFO << "        mirrored: " << angle_info->is_mirrored());

			if (angle_info->is_mirrored()) {
				log::log(INFO << "        mirrored angle: "
				              << angle_info->get_mirrored_angle().get()->get_angle_start());
			}
			else {
				for (size_t i = 0; i < angle_info->get_frame_count(); ++i) {
					auto frame_info = angle_info->get_frame(i);
					log::log(INFO << "        frame " << i << ":");
					log::log(INFO << "          texture idx: " << frame_info->get_texture_idx());
					log::log(INFO << "          subtexture idx: " << frame_info->get_subtexture_idx());
				}
			}
		}
	}

	/* Display the subtextures using the meta information */
	log::log(INFO << "Loading shaders...");

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

	auto window_size = window.get_size();

	/* Load texture image using the metafile. */
	log::log(INFO << "Loading texture image...");
	auto tex = resources::Texture2dData(*sprite_info.get_texture(0));
	auto gltex = renderer->add_texture(tex);

	/* Read location of the subtexture in the texture image */
	size_t subtexture_index = 0;
	util::Vector2s subtex_size = {tex.get_info().get_subtexture_size(subtexture_index).first,
	                              tex.get_info().get_subtexture_size(subtexture_index).second};
	auto [s_left, s_right, s_top, s_bottom] = tex.get_info().get_subtexture_coordinates(subtexture_index);
	Eigen::Vector4f subtex_coords{s_left, s_right, s_top, s_bottom};

	/* Upscale subtexture for better visibility */
	auto tex_size = tex_info.get_size();
	unsigned char upscale_factor = 3;
	float scale_x = upscale_factor * (float)subtex_size[1] / tex_size.first;
	float scale_y = upscale_factor * (float)subtex_size[0] / tex_size.second;
	auto transform1 = Eigen::Affine3f::Identity();
	transform1.prescale(Eigen::Vector3f(scale_y,
	                                    scale_x,
	                                    1.0f));

	/* Pass uniforms to the shaders.
		mv          : The upscaling matrix
		offset_tile : Subtexture coordinates (as floats relative to texture image size)
		u_id        : Identifier
		tex         : OpenGL texture reference
	*/
	auto obj1_unifs = obj_shader->new_uniform_input(
		"mv",
		transform1.matrix(),
		"offset_tile",
		subtex_coords,
		"u_id",
		1u,
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

	/* Make a framebuffer for the first render pass to draw into. The framebuffer consists of a color texture
	 * to be copied onto the back buffer in pass 2, as well as an id texture which will contain the object ids
	 * which we can later read in order to determine which object was clicked. The depth texture is required,
	 * but mostly irrelevant in this case. */
	auto color_texture = renderer->add_texture(resources::Texture2dInfo(window_size[0],
	                                                                    window_size[1],
	                                                                    resources::pixel_format::rgba8));
	auto id_texture = renderer->add_texture(resources::Texture2dInfo(window_size[0],
	                                                                 window_size[1],
	                                                                 resources::pixel_format::r32ui));
	auto depth_texture = renderer->add_texture(resources::Texture2dInfo(window_size[0],
	                                                                    window_size[1],
	                                                                    resources::pixel_format::depth24));
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
		{proj_update, obj1},
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

	/* Data retrieved from the object index texture. */
	resources::Texture2dData id_texture_data = id_texture->into_data();
	bool texture_data_valid = false;

	// TODO(Vtec234): move these into the renderer
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0, 1.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* Register callbacks */
	log::log(INFO << "Register callbacks...");

	window.add_mouse_button_callback([&](const QMouseEvent &ev) {
		auto qpos = ev.position();
		ssize_t x = qpos.x();
		ssize_t y = qpos.y();

		log::log(INFO << "Clicked at location (" << x << ", " << y << ")");
		if (!texture_data_valid) {
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
	});

	window.add_resize_callback([&](size_t w, size_t h) {
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

	/* Iterate through subtextures with left/right arrows */
	window.add_key_callback([&](const QKeyEvent &ev) {
		if (ev.type() == QEvent::KeyRelease) {
			if (ev.key() == Qt::Key_Right) {
				log::log(INFO << "Key pressed (Right arrow)");

				++subtexture_index;
				if (subtexture_index >= tex.get_info().get_subtexture_count()) {
					subtexture_index = 0;
				}
			}
			else if (ev.key() == Qt::Key_Left) {
				log::log(INFO << "Key pressed (Left arrow)");
				if (subtexture_index == 0) {
					subtexture_index = tex.get_info().get_subtexture_count() - 1;
				}
				else {
					--subtexture_index;
				}
			}
			else {
				return;
			}

			log::log(INFO << "Selected subtexture: " << subtexture_index);

			/* Rescale the transformation matrix. */
			tex_size = tex_info.get_size();
			subtex_size = {tex.get_info().get_subtexture_size(subtexture_index).first,
			               tex.get_info().get_subtexture_size(subtexture_index).second};
			scale_x = upscale_factor * (float)subtex_size[1] / tex_size.first;
			scale_y = upscale_factor * (float)subtex_size[0] / tex_size.second;

			transform1 = Eigen::Affine3f::Identity();
			transform1.prescale(Eigen::Vector3f(scale_y,
			                                    scale_x,
			                                    1.0f));

			obj1_unifs->update("mv", transform1.matrix());

			/* Pass the new subtexture coordinates. */
			auto [s_left, s_right, s_top, s_bottom] = tex.get_info().get_subtexture_coordinates(subtexture_index);
			Eigen::Vector4f subtex_coords{s_left, s_right, s_top, s_bottom};

			obj1_unifs->update("offset_tile", subtex_coords);
		}
	});
	log::log(INFO << "Success!");

	log::log(INFO << "Instructions:");
	log::log(INFO << "  1. Click on the texture pixels with LEFT MOUSE BUTTON to get the object ID");
	log::log(INFO << "  2. Press LEFT/RIGHT ARROW to cycle through available subtextures");

	while (not window.should_close()) {
		qtapp->process_events();

		renderer->render(pass1);
		renderer->render(pass2);
		window.update();

		renderer->check_error();
	}
}


/**
 * Basic test for creating a window and displaying a single-color texture
 * created by a shader.
 */
void renderer_demo_2(const util::Path &path) {
	auto qtapp = std::make_shared<gui::GuiApplicationWithLogger>();

	opengl::GlWindow window("openage renderer test", 800, 600);
	auto renderer = window.make_renderer();

	auto shaderdir = path / "assets" / "test" / "shaders";

	/* Shader for copying the framebuffer in pass 2. */
	auto display_vshader_file = (shaderdir / "demo_2_display.vert.glsl").open();
	auto display_vshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		display_vshader_file.read());
	display_vshader_file.close();

	auto display_fshader_file = (shaderdir / "demo_2_display.frag.glsl").open();
	auto display_fshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		display_fshader_file.read());
	display_fshader_file.close();

	auto display_shader = renderer->add_shader({display_vshader_src, display_fshader_src});

	auto quad = renderer->add_mesh_geometry(resources::MeshData::make_quad());
	Renderable display_stuff{
		display_shader->create_empty_input(),
		quad,
		false,
		false,
	};

	auto pass = renderer->add_render_pass({display_stuff}, renderer->get_display_target());

	while (not window.should_close()) {
		renderer->render(pass);
		window.update();
		qtapp->process_events();

		renderer->check_error();
	}
	window.close();
}


/**
 * Shows the different render stages and camera.
 *
 * @param path Path to the project rootdir.
 */
void renderer_demo_3(const util::Path &path) {
	auto qtapp = std::make_shared<gui::GuiApplicationWithLogger>();

	auto window = std::make_shared<opengl::GlWindow>("openage renderer test", 800, 600);
	auto renderer = window->make_renderer();

	// Clock required by world renderer for timing animation frames
	// (we never advance time in this demo though, so it has no significance)
	auto clock = std::make_shared<event::Clock>();

	// Camera
	// our viewport into the game world
	auto camera = std::make_shared<renderer::camera::Camera>(window->get_size());
	window->add_resize_callback([&](size_t w, size_t h) {
		camera->resize(w, h);
	});

	// Render stages
	// every stage use a different subrenderer that manages renderables,
	// shaders, textures & more.
	std::vector<std::shared_ptr<RenderPass>> render_passes{};

	// TODO: Make this optional for subrenderers?
	auto asset_manager = std::make_shared<renderer::resources::AssetManager>(renderer);

	// Renders the background
	auto skybox_renderer = std::make_shared<renderer::skybox::SkyboxRenderer>(
		window,
		renderer,
		path["assets"]["shaders"]);
	skybox_renderer->set_color(1.0f, 0.5f, 0.0f, 1.0f); // orange color

	// Renders the terrain in 3D
	auto terrain_renderer = std::make_shared<renderer::terrain::TerrainRenderer>(
		window,
		renderer,
		camera,
		path["assets"]["shaders"],
		asset_manager);

	// Renders units/buildings/other objects
	auto world_renderer = std::make_shared<renderer::world::WorldRenderer>(
		window,
		renderer,
		path["assets"]["shaders"],
		asset_manager,
		clock);

	// Store the render passes of the renderers
	// The order is important as its also the order in which they
	// are rendered and drawn onto the screen.
	render_passes.push_back(skybox_renderer->get_render_pass());
	render_passes.push_back(terrain_renderer->get_render_pass());
	render_passes.push_back(world_renderer->get_render_pass());

	// Final output on screen has its own subrenderer
	// It takes the outputs of all previous render passes
	// and blends them together
	auto screen_renderer = std::make_shared<renderer::screen::ScreenRenderer>(
		window,
		renderer,
		path["assets"]["shaders"]);
	std::vector<std::shared_ptr<renderer::RenderTarget>> targets{};
	for (auto pass : render_passes) {
		targets.push_back(pass->get_target());
	}
	screen_renderer->set_render_targets(targets);

	render_passes.push_back(screen_renderer->get_render_pass());

	window->add_resize_callback([&](size_t, size_t) {
		std::vector<std::shared_ptr<renderer::RenderTarget>> targets{};
		for (size_t i = 0; i < render_passes.size() - 1; ++i) {
			targets.push_back(render_passes[i]->get_target());
		}
		screen_renderer->set_render_targets(targets);
	});

	// Create some entities to populate the scene
	auto render_factory = std::make_shared<RenderFactory>(terrain_renderer, world_renderer);

	// Terrain
	auto terrain0 = render_factory->add_terrain_render_entity();

	// Fill a 10x10 terrain grid with height values
	auto terrain_size = util::Vector2s{10, 10};
	std::vector<float> height_map{};
	height_map.reserve(terrain_size[0] * terrain_size[1]);
	for (size_t i = 0; i < terrain_size[0] * terrain_size[1]; ++i) {
		height_map.push_back(0.0f);
	}

	// Create "test bumps" in the terrain to check if rendering works
	height_map[11] = 1.0f;
	height_map[23] = 2.3f;
	height_map[42] = 4.2f;
	height_map[69] = 6.9f; // nice

	// A hill
	height_map[55] = 3.0f; // center
	height_map[45] = 2.0f; // bottom left slope
	height_map[35] = 1.0f;
	height_map[56] = 1.0f; // bottom right slope (little steeper)
	height_map[65] = 2.0f; // top right slope
	height_map[75] = 1.0f;
	height_map[54] = 2.0f; // top left slope
	height_map[53] = 1.0f;

	// send the terrain data to the terrain renderer
	terrain0->update(terrain_size,
	                 height_map,
	                 path["assets"]["test"]["textures"]["test_terrain.terrain"]);

	// Game entities
	auto world0 = render_factory->add_world_render_entity();
	world0->update(0, util::Vector3f(3.0f, 3.0f, 0.0f), path["assets"]["test"]["textures"]["test_gaben.sprite"]);

	auto world1 = render_factory->add_world_render_entity();
	world1->update(1, util::Vector3f(7.5f, 6.0f, 0.0f), path["assets"]["test"]["textures"]["test_missing.sprite"]);

	// Zoom in/out with mouse wheel
	window->add_mouse_wheel_callback([&](const QWheelEvent &ev) {
		auto delta = ev.angleDelta().y() / 120;

		if (delta < 0) {
			camera->zoom_out(0.05f);
		}
		else if (delta > 0) {
			camera->zoom_in(0.05f);
		}
	});

	// Move around the scene with WASD
	window->add_key_callback([&](const QKeyEvent &ev) {
		auto key = ev.key();

		switch (key) {
		case Qt::Key_W: { // forward
			camera->move_rel(Eigen::Vector3f(1.0f, 0.0f, 1.0f), 0.5f);
		} break;
		case Qt::Key_A: { // left
			// half the speed because the relationship between forward/back and
			// left/right is 1:2 in our ortho projection.
			camera->move_rel(Eigen::Vector3f(1.0f, 0.0f, -1.0f), 0.25f);
		} break;
		case Qt::Key_S: { // back
			camera->move_rel(Eigen::Vector3f(-1.0f, 0.0f, -1.0f), 0.5f);
		} break;
		case Qt::Key_D: { // right
			// half the speed because the relationship between forward/back and
			// left/right is 1:2 in our ortho projection.
			camera->move_rel(Eigen::Vector3f(-1.0f, 0.0f, 1.0f), 0.25f);
		} break;
		default:
			break;
		}
	});

	log::log(INFO << "Instructions:");
	log::log(INFO << "  1. Move the camera with WASD");
	log::log(INFO << "  2. Zoom in and out with MOUSE WHEEL");

	while (not window->should_close()) {
		qtapp->process_events();

		// Update the renderables of the subrenderers
		// camera zoom/position changes are also handled in here
		terrain_renderer->update();
		world_renderer->update();

		// Draw everything
		for (auto pass : render_passes) {
			renderer->render(pass);
		}

		renderer->check_error();

		// Display final output on screen
		window->update();
	}
	window->close();
}


/**
 * Demo the timing of animation keyframes with the simulation clock.
 *
 * @param path Path to the openage asset directory.
 */
void renderer_demo_4(const util::Path &path) {
	auto qtapp = std::make_shared<gui::GuiApplicationWithLogger>();

	opengl::GlWindow window("openage renderer test", 800, 600);
	auto renderer = window.make_renderer();

	/* Clock for timed display */
	auto clock = event::Clock();

	/* Controls whether animations are use "real" time for frame timings (if true)
	   or the actual simulation time (if false). When simulation time is used,
	   the animation speed increases with the clock speed.*/
	bool real_time_animation = true;

	/* Load animation file */
	auto sprite_path = path / "assets/test/textures/test_animation.sprite";
	auto sprite_info = renderer::resources::parser::parse_sprite_file(sprite_path);

	/* Display the subtextures using the meta information */
	log::log(INFO << "Loading shaders...");

	auto shaderdir = path / "assets" / "test" / "shaders";

	/* Shader for individual objects in pass 1. */
	auto obj_vshader_file = (shaderdir / "demo_4_obj.vert.glsl").open();
	auto obj_vshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		obj_vshader_file.read());
	obj_vshader_file.close();

	auto obj_fshader_file = (shaderdir / "demo_4_obj.frag.glsl").open();
	auto obj_fshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		obj_fshader_file.read());
	obj_fshader_file.close();

	/* Shader for copying the framebuffer in pass 2. */
	auto display_vshader_file = (shaderdir / "demo_4_display.vert.glsl").open();
	auto display_vshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		display_vshader_file.read());
	display_vshader_file.close();

	auto display_fshader_file = (shaderdir / "demo_4_display.frag.glsl").open();
	auto display_fshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		display_fshader_file.read());
	display_fshader_file.close();

	auto obj_shader = renderer->add_shader({obj_vshader_src, obj_fshader_src});
	auto display_shader = renderer->add_shader({display_vshader_src, display_fshader_src});

	auto window_size = window.get_size();

	/* Load texture image using the metafile. */
	log::log(INFO << "Loading texture image...");
	auto tex = resources::Texture2dData(*sprite_info.get_texture(0));
	auto gltex = renderer->add_texture(tex);

	/* Read location of the first subtexture in the texture image */
	size_t subtexture_index = 0;
	util::Vector2s subtex_size = {tex.get_info().get_subtexture_size(subtexture_index).first,
	                              tex.get_info().get_subtexture_size(subtexture_index).second};
	auto [s_left, s_right, s_top, s_bottom] = tex.get_info().get_subtexture_coordinates(subtexture_index);
	Eigen::Vector4f subtex_coords{s_left, s_right, s_top, s_bottom};

	/* Upscale subtexture for better visibility */
	auto tex_size = sprite_info.get_texture(0)->get_size();
	unsigned char upscale_factor = 3;
	float scale_x = upscale_factor * (float)subtex_size[1] / tex_size.first;
	float scale_y = upscale_factor * (float)subtex_size[0] / tex_size.second;
	auto transform1 = Eigen::Affine3f::Identity();
	transform1.prescale(Eigen::Vector3f(scale_y,
	                                    scale_x,
	                                    1.0f));

	/* Pass uniforms to the shaders.
		mv          : The upscaling matrix
		offset_tile : Subtexture coordinates (as floats relative to texture image size)
		u_id        : Identifier
		tex         : OpenGL texture reference
	*/
	auto obj1_unifs = obj_shader->new_uniform_input(
		"mv",
		transform1.matrix(),
		"offset_tile",
		subtex_coords,
		"u_id",
		1u,
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

	/* Make a framebuffer for the first render pass to draw into. The framebuffer consists of a color texture
	 * to be copied onto the back buffer in pass 2, as well as an id texture which will contain the object ids
	 * which we can later read in order to determine which object was clicked. The depth texture is required,
	 * but mostly irrelevant in this case. */
	auto color_texture = renderer->add_texture(resources::Texture2dInfo(window_size[0],
	                                                                    window_size[1],
	                                                                    resources::pixel_format::rgba8));
	auto fbo = renderer->create_texture_target({color_texture});

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
		{proj_update, obj1},
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

	window.add_resize_callback([&](size_t w, size_t h) {
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
		fbo = renderer->create_texture_target({color_texture});

		color_texture_unif->update("color_texture", color_texture);

		pass1->set_target(fbo);
	});

	// Control simulation clock
	window.add_key_callback([&](const QKeyEvent &ev) {
		auto key = ev.key();

		switch (key) {
		case Qt::Key_Space: {
			if (clock.get_state() == event::ClockState::RUNNING) {
				clock.stop();
				log::log(INFO << "Stopped simulation at " << clock.get_time() << " (real = " << clock.get_real_time() << ")");
			}
			else if (clock.get_state() == event::ClockState::STOPPED) {
				clock.resume();
				log::log(INFO << "Resumed simulation at " << clock.get_time() << " (real = " << clock.get_real_time() << ")");
			}
		} break;
		case Qt::Key_Return: {
			real_time_animation = not real_time_animation;
			if (real_time_animation) {
				log::log(INFO << "Animation speed switched to REAL time");
			}
			else {
				log::log(INFO << "Animation speed switched to SIMULATION time");
			}
		} break;
		case Qt::Key_Minus: {
			clock.set_speed(clock.get_speed() - 0.5);
			log::log(INFO << "Increased clock speed to: " << clock.get_speed());
		} break;
		case Qt::Key_Plus: {
			clock.set_speed(clock.get_speed() + 0.5);
			log::log(INFO << "Decreased clock speed to: " << clock.get_speed());
		} break;
		default:
			break;
		}
	});

	log::log(INFO << "Instructions:");
	log::log(INFO << "  1. Press SPACE to pause/resume simulation clock");
	log::log(INFO << "  2. Press PLUS and MINUS keys to increase/decrease simulation speed");
	log::log(INFO << "  2. Press RETURN to toggle between real time and simulation time for animation speed");

	/* Get layer and angle that we want to display. For this texture, there is only
	   one layer and one angle. */
	size_t frame_idx = 0;
	auto layer = sprite_info.get_layer(0);
	auto angle = layer.get_angle(0);
	auto timing = layer.get_frame_timing();

	clock.start();
	while (not window.should_close()) {
		qtapp->process_events();

		/* Check simulation time to see if we have to switch to the next frame. */
		clock.update_time();
		size_t timed_frame_idx = 0;
		if (real_time_animation) {
			// use real time for animation speed
			timed_frame_idx = timing->get_mod(clock.get_real_time(), 0);
		}
		else {
			// use simulation time for animation speed (which is potentially sped up/slowed down)
			timed_frame_idx = timing->get_mod(clock.get_time(), 0);
		}

		/* Update uniforms if new frame should be displayed */
		if (timed_frame_idx != frame_idx) {
			frame_idx = timed_frame_idx;

			/* Get index of texture and subtexture where the frame's pixels are located */
			auto frame_info = angle->get_frame(frame_idx);
			// auto tex_idx = frame_info->get_texture_idx();  // already loaded as 'tex'
			auto subtex_idx = frame_info->get_subtexture_idx();

			/* Rescale the transformation matrix. */
			tex_size = tex.get_info().get_size();
			subtex_size = {tex.get_info().get_subtexture_size(subtex_idx).first,
			               tex.get_info().get_subtexture_size(subtex_idx).second};
			scale_x = upscale_factor * (float)subtex_size[1] / tex_size.first;
			scale_y = upscale_factor * (float)subtex_size[0] / tex_size.second;

			transform1 = Eigen::Affine3f::Identity();
			transform1.prescale(Eigen::Vector3f(scale_y,
			                                    scale_x,
			                                    1.0f));

			obj1_unifs->update("mv", transform1.matrix());

			/* Pass the new subtexture coordinates. */
			auto [s_left, s_right, s_top, s_bottom] = tex.get_info().get_subtexture_coordinates(subtex_idx);
			Eigen::Vector4f subtex_coords{s_left, s_right, s_top, s_bottom};

			obj1_unifs->update("offset_tile", subtex_coords);

			/* Log subtex index and time */
			log::log(INFO << "Switch to subtex: " << timed_frame_idx);
			log::log(INFO << "Time: " << clock.get_time() << " (real = " << clock.get_real_time() << ")");
		}

		renderer->render(pass1);
		renderer->render(pass2);
		window.update();

		renderer->check_error();
	}
	window.close();
}


void renderer_demo(int demo_id, const util::Path &path) {
	switch (demo_id) {
	case 0:
		renderer_demo_0(path);
		break;

	case 1:
		renderer_demo_1(path);
		break;

	case 2:
		renderer_demo_2(path);
		break;

	case 3:
		renderer_demo_3(path);
		break;

	case 4:
		renderer_demo_4(path);
		break;

	default:
		log::log(MSG(err) << "Unknown renderer demo requested: " << demo_id << ".");
		break;
	}
}

} // namespace openage::renderer::tests
