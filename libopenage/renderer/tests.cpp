// Copyright 2015-2021 the openage authors. See copying.md for legal info.

#include "tests.h"

#include <cstdlib>
#include <eigen3/Eigen/Dense>
#include <epoxy/gl.h>
#include <functional>
#include <memory>
#include <unordered_map>

#include "../error/error.h"
#include "../log/log.h"
#include "../util/math_constants.h"
#include "geometry.h"
#include "opengl/window.h"
#include "resources/mesh_data.h"
#include "resources/parser/parse_sprite.h"
#include "resources/parser/parse_texture.h"
#include "resources/shader_source.h"
#include "resources/texture_data.h"
#include "shader_program.h"
#include "texture.h"


namespace openage {
namespace renderer {
namespace tests {

// Macro for debugging OpenGL state.
#define DBG_GL(txt) \
	printf("before %s\n", txt); \
	opengl::GlContext::check_error(); \
	printf("after %s\n", txt);

/**
 * Loads a texture image and applies various transformation and display options
 * using renderer functions.
 *
 * @param path Path to the openage asset directory.
 */
void renderer_demo_0(const util::Path &path) {
	opengl::GlWindow window("openage renderer test", 800, 600);
	auto renderer = window.make_renderer();

	/* Shader for individual objects in pass 1. */
	auto obj_vshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		R"s(
#version 330

layout(location=0) in vec2 position;
layout(location=1) in vec2 uv;
uniform mat4 mv;
uniform mat4 proj;
out vec2 v_uv;

void main() {
	gl_Position = proj * mv * vec4(position, 0.0, 1.0);
	v_uv = vec2(uv.x, 1.0 - uv.y);
}
)s");

	auto obj_fshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		R"s(
#version 330

in vec2 v_uv;
uniform sampler2D tex;
uniform uint u_id;

layout(location=0) out vec4 col;
layout(location=1) out uint id;

void main() {
	vec4 tex_val = texture(tex, v_uv);
	if (tex_val.a == 0) {
		discard;
	}
	col = tex_val;
	id = u_id;
}
)s");

	/* Shader for copying the framebuffer in pass 2. */
	auto display_vshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		R"s(
#version 330

layout(location=0) in vec2 position;
layout(location=1) in vec2 uv;
out vec2 v_uv;

void main() {
	gl_Position =  vec4(position, 0.0, 1.0);
	v_uv = uv;
}
)s");

	auto display_fshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		R"s(
#version 330

uniform sampler2D color_texture;

in vec2 v_uv;
out vec4 col;

void main() {
	col = texture(color_texture, v_uv);
}
)s");

	auto obj_shader = renderer->add_shader({obj_vshader_src, obj_fshader_src});
	auto display_shader = renderer->add_shader({display_vshader_src, display_fshader_src});

	/* Texture for the clickable objects. */
	auto tex = resources::Texture2dData(path / "/assets/gaben.png");
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

	window.add_mouse_button_callback([&](SDL_MouseButtonEvent const &ev) {
		auto x = ev.x;
		auto y = ev.y;

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
		opengl::GlContext::check_error();
	}
}

/**
 * Loads .sprite and .texture meta information files and displays
 * subtextures from the attached texture images.
 *
 * @param path Path to the openage asset directory.
 */
void renderer_demo_1(const util::Path &path) {
	opengl::GlWindow window("openage renderer test", 800, 600);
	auto renderer = window.make_renderer();

	/* Load texture file standalone. */
	auto tex_path = path / "assets/tests/renderer/1/test_texture.texture";
	auto tex_info = renderer::resources::parser::parse_texture_file(tex_path);

	log::log(INFO << "Loaded texture " << tex_path.resolve_native_path());
	log::log(INFO << "  imagefile: " << tex_info.get_image_path().get()->resolve_native_path());
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
	auto sprite_path = path / "assets/tests/renderer/1/test_animation.sprite";
	auto sprite_info = renderer::resources::parser::parse_sprite_file(sprite_path);

	log::log(INFO << "Loaded animation " << sprite_path.resolve_native_path());
	log::log(INFO << "  texture count: " << sprite_info.get_texture_count());
	for (size_t i = 0; i < sprite_info.get_texture_count(); ++i) {
		log::log(INFO << "    texture " << i << ": "
		              << sprite_info.get_texture(i).get_image_path().get()->resolve_native_path());
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
			log::log(INFO << "        degree: " << angle_info.get_angle_start());
			log::log(INFO << "        frame count: " << angle_info.get_frame_count());
			log::log(INFO << "        mirrored: " << angle_info.is_mirrored());

			if (angle_info.is_mirrored()) {
				log::log(INFO << "        mirrored angle: "
				              << angle_info.get_mirrored_angle().get()->get_angle_start());
			}
			else {
				for (size_t i = 0; i < angle_info.get_frame_count(); ++i) {
					auto frame_info = angle_info.get_frame(i);
					log::log(INFO << "        frame " << i << ":");
					log::log(INFO << "          texture idx: " << frame_info.get_texture_idx());
					log::log(INFO << "          subtexture idx: " << frame_info.get_subtexture_idx());
				}
			}
		}
	}

	/* Display the subtextures using the meta information */
	log::log(INFO << "Loading shaders...");

	/* Shader for individual objects in pass 1. */
	auto obj_vshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		R"s(
#version 330

layout(location=0) in vec2 position;
layout(location=1) in vec2 uv;

uniform mat4 mv;
uniform mat4 proj;
uniform vec4 offset_tile;

float width = offset_tile.y - offset_tile.x;
float height = offset_tile.w - offset_tile.z;

out vec2 v_uv;

void main() {
	gl_Position = proj * mv * vec4(position, 0.0, 1.0);
    v_uv = vec2((uv.x * width) + offset_tile.x, (((1.0 - uv.y) * height) + offset_tile.z));
}
)s");

	auto obj_fshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		R"s(
#version 330

in vec2 v_uv;
uniform sampler2D tex;
uniform uint u_id;

layout(location=0) out vec4 col;
layout(location=1) out uint id;

void main() {
	vec4 tex_val = texture(tex, v_uv);
	int alpha = int(round(tex_val.a * 255));
	switch (alpha) {
		case 0:
		discard;
		break;
		case 254:
		col = vec4(1.0f, 0.0f, 0.0f, 1.0f);
		break;
		case 252:
		col = vec4(0.0f, 1.0f, 0.0f, 1.0f);
		break;
		case 250:
		col = vec4(0.0f, 0.0f, 1.0f, 1.0f);
		break;
		default:
	    col = tex_val;
		break;
	}
	id = u_id;
}
)s");

	/* Shader for copying the framebuffer in pass 2. */
	auto display_vshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		R"s(
#version 330

layout(location=0) in vec2 position;
layout(location=1) in vec2 uv;
out vec2 v_uv;

void main() {
	gl_Position = vec4(position, 0.0, 1.0);
	v_uv = uv;
}
)s");

	auto display_fshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		R"s(
#version 330

uniform sampler2D color_texture;

in vec2 v_uv;
out vec4 col;

void main() {
	col = texture(color_texture, v_uv);
}
)s");

	auto obj_shader = renderer->add_shader({obj_vshader_src, obj_fshader_src});
	auto display_shader = renderer->add_shader({display_vshader_src, display_fshader_src});

	auto window_size = window.get_size();

	/* Load texture image using the metafile. */
	log::log(INFO << "Loading texture image...");
	auto tex = resources::Texture2dData(sprite_info.get_texture(0));
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

	window.add_mouse_button_callback([&](SDL_MouseButtonEvent const &ev) {
		auto x = ev.x;
		auto y = ev.y;

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
	window.add_key_callback([&](SDL_KeyboardEvent event) {
		if (event.type == SDL_KEYUP) {
			if (event.keysym.sym == SDLK_RIGHT) {
				log::log(INFO << "Key pressed (Right arrow)");

				++subtexture_index;
				if (subtexture_index >= tex.get_info().get_subtexture_count()) {
					subtexture_index = 0;
				}
			}
			else if (event.keysym.sym == SDLK_LEFT) {
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
		renderer->render(pass1);
		renderer->render(pass2);
		window.update();
		opengl::GlContext::check_error();
	}
}

void renderer_demo(int demo_id, const util::Path &path) {
	switch (demo_id) {
	case 0:
		renderer_demo_0(path);
		break;

	case 1:
		renderer_demo_1(path);
		break;

	default:
		log::log(MSG(err) << "Unknown renderer demo requested: " << demo_id << ".");
		break;
	}
}

} // namespace tests
} // namespace renderer
} // namespace openage
