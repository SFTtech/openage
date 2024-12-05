// Copyright 2015-2024 the openage authors. See copying.md for legal info.

#include "demo_4.h"

#include <eigen3/Eigen/Dense>
#include <QKeyEvent>

#include "renderer/demo/util.h"
#include "renderer/gui/integration/public/gui_application_with_logger.h"
#include "renderer/opengl/window.h"
#include "renderer/render_pass.h"
#include "renderer/render_target.h"
#include "renderer/resources/animation/angle_info.h"
#include "renderer/resources/animation/frame_info.h"
#include "renderer/resources/frame_timing.h"
#include "renderer/resources/parser/parse_sprite.h"
#include "renderer/resources/shader_source.h"
#include "renderer/resources/texture_data.h"
#include "renderer/shader_program.h"
#include "time/clock.h"


namespace openage::renderer::tests {
void renderer_demo_4(const util::Path &path) {
	auto qtapp = std::make_shared<gui::GuiApplicationWithLogger>();

	window_settings settings;
	settings.width = 800;
	settings.height = 600;
	settings.debug = true;
	opengl::GlWindow window("openage renderer test", settings);
	auto renderer = window.make_renderer();

	/* Clock for timed display */
	auto clock = time::Clock();

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
	auto subtex_size = tex.get_info().get_subtex_info(subtexture_index).get_size();
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

	if (not check_uniform_completeness({proj_update, obj1, display_obj})) {
		log::log(WARN << "Uniforms not complete.");
	}

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
		fbo = renderer->create_texture_target({color_texture});

		color_texture_unif->update("color_texture", color_texture);

		pass1->set_target(fbo);
	});

	// Control simulation clock
	window.add_key_callback([&](const QKeyEvent &ev) {
		if (ev.type() == QEvent::KeyRelease) {
			auto key = ev.key();

			switch (key) {
			case Qt::Key_Space: {
				if (clock.get_state() == time::ClockState::RUNNING) {
					clock.pause();
					log::log(INFO << "Stopped simulation at " << clock.get_time() << " (real = " << clock.get_real_time() << ")");
				}
				else if (clock.get_state() == time::ClockState::PAUSED) {
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
				log::log(INFO << "Decreased clock speed to: " << clock.get_speed());
			} break;
			case Qt::Key_Plus: {
				clock.set_speed(clock.get_speed() + 0.5);
				log::log(INFO << "Increased clock speed to: " << clock.get_speed());
			} break;
			default:
				break;
			}
		}
	});

	log::log(INFO << "Instructions:");
	log::log(INFO << "  1. Press SPACE to pause/resume simulation clock");
	log::log(INFO << "  2. Press PLUS and MINUS keys to increase/decrease simulation speed");
	log::log(INFO << "  3. Press RETURN to toggle between real time and simulation time for animation speed");

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
			timed_frame_idx = timing->get_frame(clock.get_real_time(), 0);
		}
		else {
			// use simulation time for animation speed (which is potentially sped up/slowed down)
			timed_frame_idx = timing->get_frame(clock.get_time(), 0);
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
			subtex_size = tex.get_info().get_subtex_info(subtex_idx).get_size();
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

} // namespace openage::renderer::tests
