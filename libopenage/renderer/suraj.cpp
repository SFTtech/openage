// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include <cstdlib>
#include <epoxy/gl.h>
#include <functional>
#include <unordered_map>
#include <memory>
#include <eigen3/Eigen/Dense>

#include "../log/log.h"
#include "../error/error.h"
#include "resources/shader_source.h"
#include "resources/texture_data.h"
#include "resources/mesh_data.h"
#include "texture.h"
#include "shader_program.h"
#include "geometry.h"
#include "opengl/window.h"
#include "opengl/context.h"
//#include "perspective.h"
#include "opengl/sprite.h"
#include "opengl/shader_program.h"

namespace openage {
namespace renderer {
namespace tests {



void renderer_demo_0(util::Path path) {
	opengl::GlWindow window("openage renderer test", { 1366, 768 } );

	auto renderer = window.make_renderer();

	auto vshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		R"s(
#version 330

layout(location=0) in vec2 position;
layout(location=1) in vec2 uv;
uniform mat4 mvp;
out vec2 v_uv;

void main() {
	gl_Position = mvp * vec4(position, 0.0, 1.0);

  v_uv = vec2(uv.x, uv.y);
}
)s");

	auto fshader_src = resources::ShaderSource(
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
	id = u_id + 1u;
}
)s");

	auto vshader_display_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		R"s(
#version 330

layout(location=0) in vec2 position;
layout(location=1) in vec2 uv;
uniform mat4 proj;
out vec2 v_uv;

void main() {
	gl_Position = proj * vec4(position, 0.0, 1.0);
	v_uv = uv;
}
)s");

	auto fshader_display_src = resources::ShaderSource(
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
	auto size = window.get_size();
	auto shader = renderer->add_shader( { vshader_src, fshader_src } );
	auto shader_display = renderer->add_shader( { vshader_display_src, fshader_display_src } );

	//start of experimental part




	//end of experimental area
	float aspect = (float)size.y/(float)size.x;

	//start of experimental area
	opengl::Sprite sprite;
	auto test_obj = sprite.create(path, true,"/assets/terrain/textures/g_ds2_00_color_1.png",0, false, shader, renderer, aspect, 0.6, Eigen::Vector3f(0.0,0.0,0.0));
	//auto test_obj2 = sprite.create(path,false, "/assets/converted/graphics/21.slp.png",0, true, shader, renderer, aspect, 0.3, Eigen::Vector3f(0.0,0.0,0.0));
	//auto test_obj3 = sprite.create(path,false, "/assets/converted/graphics/739.slp.png",0, true, shader, renderer, aspect, 0.23, Eigen::Vector3f(0.5,0.0,0.0));
	auto test_obj4 = sprite.create(path,true, "/assets/converted/graphics/209.slp.png",20, true, shader, renderer, aspect, 1, Eigen::Vector3f(0.0,0.0,0.0));
	//auto test_obj5 = sprite.create(path,false, "/assets/converted/graphics/305.slp.png",0, false, shader, renderer, aspect, 0.5, Eigen::Vector3f(-0.4,0.0,0.0));
	//auto test_obj6 = sprite.create(path,false, "/assets/converted/graphics/209.slp.png",0, true, shader, renderer, aspect, 0.1, Eigen::Vector3f(0.0,0.5,0.0));
	//auto test_obj7 = sprite.create(path,false, "/assets/converted/graphics/209.slp.png",0, true, shader, renderer, aspect, 0.1, Eigen::Vector3f(0.0,0.5,0.0));
	auto test_obj6 = sprite.create(path, true,"/assets/terrain/textures/g_ds2_00_color_1.png",0, false, shader, renderer, aspect, 0.6, Eigen::Vector3f(0.7,0.7,0.0));



	RenderPass_test display_pass_test {
		{ test_obj,test_obj6,test_obj4},
		renderer->get_display_target(),
	};


	log::log(INFO << "Size of the Window "<<size.x<<"X"<<size.y);
	auto color_texture = renderer->add_texture(resources::TextureInfo(size.x, size.y, resources::pixel_format::rgba8));
	auto id_texture = renderer->add_texture(resources::TextureInfo(size.x, size.y, resources::pixel_format::r32ui));
	auto depth_texture = renderer->add_texture(resources::TextureInfo(size.x, size.y, resources::pixel_format::depth24));
	//one of the targets
	auto fbo = renderer->create_texture_target( { color_texture.get(), id_texture.get(), depth_texture.get() } );

	auto color_texture_uniform = shader_display->new_uniform_input("color_texture", color_texture.get());

	resources::TextureData id_texture_data = id_texture->into_data();
	bool texture_data_valid = false;

	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0, 1.0);
	// what is this
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	window.add_mouse_button_callback([&] (SDL_MouseButtonEvent const& ev) {
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
				//no renderable at given location
				log::log(INFO << "Clicked at non existent object");
				return;
			}
			id--; //real id is id-1
			log::log(INFO << "Object number " << id << " clicked.");
			renderer->display_into_data().store(path / "/assets/screen.png");
		} );

	window.add_resize_callback([&] (coord::window new_size) {
			// Calculate projection matrix
			float aspectRatio = float(new_size.x)/float(new_size.y);
			float xScale = 1.0/aspectRatio;

			Eigen::Matrix4f pmat;
			pmat << xScale, 0, 0, 0,
							0, 1, 0, 0,
							0, 0, 1, 0,
							0, 0, 0, 1;

			// resize fbo
			color_texture = renderer->add_texture(resources::TextureInfo(new_size.x, new_size.y, resources::pixel_format::rgba8));
			id_texture = renderer->add_texture(resources::TextureInfo(new_size.x, new_size.y, resources::pixel_format::r32ui));
			depth_texture = renderer->add_texture(resources::TextureInfo(new_size.x, new_size.y, resources::pixel_format::depth24));
			fbo = renderer->create_texture_target( { color_texture.get(), id_texture.get(), depth_texture.get() } );
			texture_data_valid = false;

			shader_display->update_uniform_input(color_texture_uniform.get(), "color_texture", color_texture.get(), "proj", pmat);
			//pass.target = fbo.get();
		} );

	while (!window.should_close()) {
		//renderer->render_test(fbo_pass_test);
		renderer->render_test(display_pass_test);
		window.update();
		window.get_context()->check_error();
	}
}


void renderer_demo(int demo_id, util::Path path) {
	switch (demo_id) {
	case 0:
		renderer_demo_0(path);
		break;

	default:
		log::log(MSG(err) << "unknown renderer demo " << demo_id << " requested.");
		break;
	}
}

}}} // namespace openage::renderer::tests
