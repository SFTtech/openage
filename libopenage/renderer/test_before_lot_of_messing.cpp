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
	auto transform1 = Eigen::Affine3f::Identity();
	transform1.prescale(Eigen::Vector3f(0.3f, 0.3f,0.0f));
	Eigen::AngleAxis<float> aaz(180*(3.14159f / 180.0f), Eigen::Vector3f(0.0,0.0,1.0));
	//transform1.prerotate(aaz);
	float zfar = 0.1,znear  = 100.0;
	Eigen::Matrix4f dimet;
	    dimet << 	1.0,-1,0,0,
	            	0.707,0.707,0.75,0,
	            	0,0,0,0,
	            	0,0,0,1;

	Eigen::Matrix4f pers;
	pers << 	(float)size.y/(float)size.x,0,0,0,
						0,1.0,0,0,
						0,  0,-2*(float)(zfar*znear)/(zfar-znear),0,
					  0,0,  -1*(float)(zfar+znear)/(zfar-znear),  1;


					 Eigen::IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
					 log::log(INFO << "data inside the pers matrix "<<pers.format(CleanFmt));

	//start of experimental area
	opengl::Sprite sprite;
	auto test_obj = sprite.create(path, "/assets/terrain/textures/g_ds2_00_color_1.png",0, false, shader, renderer, aspect, 0.6, Eigen::Vector3f(0.0,0.0,0.0));
	//auto uniform2 = sprite.make_unif(path, "/assets/terrain/textures/g_ds2_00_color_1.png", shader, renderer);
	//end of experimental area
	auto transform_temp = pers*transform1;
	//transform_temp.pretranslate(Eigen::Vector3f(0.4f, 0.4f, 0.0f));
	auto tex = resources::TextureData(path / "/assets/converted/graphics/21.slp.png",true);
	auto gltex = renderer->add_texture(tex);
	auto unif_in1 = shader->new_uniform_input(
		"mvp", transform_temp.matrix(),
		//"color", Eigen::Vector4f(0.0f, 1.0f, 0.0f, 1.0f),
		"u_id", 2u,
		"tex", gltex.get()
	);

	//start of experimental part
	//let's start by making just the uniform variable outside




	//end of experimental area
	resources::TextureInfo tex_info = tex.get_info();
	size_t count = tex_info.get_subtexture_count();
	log::log(INFO << "Number of sprites in spritesheet : "<<count);
	std::tuple<float, float, float, float> sprite_coord = tex_info.get_subtexture_coordinates(0);
	log::log(INFO << "Coordinate of Sprite 4 is : "<< std::get<0>(sprite_coord)<<"  "<<std::get<1>(sprite_coord)<<"  "<<std::get<2>(sprite_coord)<<"  "<<std::get<3>(sprite_coord));
	float left = std::get<0>(sprite_coord);
	float right = std::get<1>(sprite_coord);
	float bottom = std::get<2>(sprite_coord);
	float top = std::get<3>(sprite_coord);
	//float aspect = (float)window.get_size().x/window.get_size().y;
	std::array<float, 16> quad_data2 = { {
			-1.0f, 1.0f, left, bottom,//bottom left
			-1.0f, -1.0f, left, top,//top left
			1.0f, 1.0f, right, bottom,//bottom right
			1.0f, -1.0f, right, top//top right
		} };
	/*static constexpr const std::array<float, 16> quad_data3 = { {
			-1.0f, 1.0f, 0.0f, 0.25f,
			-1.0f, -1.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.25f, 0.25f,
			1.0f, -1.0f, 0.25f, 0.0f
		} };*/
	auto quad = renderer->add_mesh_geometry(resources::MeshData::make_quad());
	auto quad2 = renderer->add_mesh_geometry(resources::MeshData::make_quad(quad_data2));

	Renderable obj2 {
		unif_in1.get(),
		quad2.get(),
		true,
		true,
	};


	log::log(INFO << "Size of the Window "<<size.x<<"X"<<size.y);
	auto color_texture = renderer->add_texture(resources::TextureInfo(size.x, size.y, resources::pixel_format::rgba8));
	auto id_texture = renderer->add_texture(resources::TextureInfo(size.x, size.y, resources::pixel_format::r32ui));
	auto depth_texture = renderer->add_texture(resources::TextureInfo(size.x, size.y, resources::pixel_format::depth24));
	auto fbo = renderer->create_texture_target( { color_texture.get(), id_texture.get(), depth_texture.get() } );

	auto color_texture_uniform = shader_display->new_uniform_input("color_texture", color_texture.get());
	Renderable display_obj {
		color_texture_uniform.get(),
		quad.get(),
		false,
		false,
	};

	RenderPass pass {
		{ obj2 },
		fbo.get()
	};
	log::log(INFO << "Above or below");
	RenderPass display_pass {
		{ obj2 },
		renderer->get_display_target(),
	};
log::log(INFO << "Definitely above");
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
			pass.target = fbo.get();
		} );

	while (!window.should_close()) {
		//renderer->render(pass);
		log::log(INFO << "Or is it here");
		renderer->render(display_pass);
		log::log(INFO << "Definitely above");
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
