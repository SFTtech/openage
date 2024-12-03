// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#include "demo_2.h"

#include <eigen3/Eigen/Dense>
#include <epoxy/gl.h>
#include <QMouseEvent>

#include "renderer/demo/util.h"
#include "renderer/gui/integration/public/gui_application_with_logger.h"
#include "renderer/opengl/window.h"
#include "renderer/render_pass.h"
#include "renderer/render_target.h"
#include "renderer/resources/animation/angle_info.h"
#include "renderer/resources/animation/frame_info.h"
#include "renderer/resources/parser/parse_sprite.h"
#include "renderer/resources/parser/parse_texture.h"
#include "renderer/resources/shader_source.h"
#include "renderer/resources/texture_data.h"
#include "renderer/shader_program.h"
#include "renderer/texture.h"


namespace openage::renderer::tests {

void renderer_demo_2(const util::Path &path) {
	auto qtapp = std::make_shared<gui::GuiApplicationWithLogger>();

	window_settings settings;
	settings.width = 800;
	settings.height = 600;
	settings.debug = true;
	opengl::GlWindow window("openage renderer test", settings);
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
	log::log(INFO << "  subtex count: " << tex_info.get_subtex_count());

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
	auto obj_vshader_file = (shaderdir / "demo_2_obj.vert.glsl").open();
	auto obj_vshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		obj_vshader_file.read());
	obj_vshader_file.close();

	auto obj_fshader_file = (shaderdir / "demo_2_obj.frag.glsl").open();
	auto obj_fshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		obj_fshader_file.read());
	obj_fshader_file.close();

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

	auto obj_shader = renderer->add_shader({obj_vshader_src, obj_fshader_src});
	auto display_shader = renderer->add_shader({display_vshader_src, display_fshader_src});

	auto window_size = window.get_size();

	/* Load texture image using the metafile. */
	log::log(INFO << "Loading texture image...");
	auto tex = resources::Texture2dData(*sprite_info.get_texture(0));
	auto gltex = renderer->add_texture(tex);

	/* Read location of the subtexture in the texture image */
	size_t subtexture_index = 0;
	auto subtex_size = tex.get_info().get_subtex_info(subtexture_index).get_size();
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

	if (not check_uniform_completeness({proj_update, obj1, display_obj})) {
		log::log(WARN << "Uniforms not complete.");
	}

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

	/* Iterate through subtextures with left/right arrows */
	window.add_key_callback([&](const QKeyEvent &ev) {
		if (ev.type() == QEvent::KeyRelease) {
			if (ev.key() == Qt::Key_Right) {
				log::log(INFO << "Key pressed (Right arrow)");

				++subtexture_index;
				if (subtexture_index >= tex.get_info().get_subtex_count()) {
					subtexture_index = 0;
				}
			}
			else if (ev.key() == Qt::Key_Left) {
				log::log(INFO << "Key pressed (Left arrow)");
				if (subtexture_index == 0) {
					subtexture_index = tex.get_info().get_subtex_count() - 1;
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
			subtex_size = tex.get_info().get_subtex_info(subtexture_index).get_size();
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

} // namespace openage::renderer::tests
