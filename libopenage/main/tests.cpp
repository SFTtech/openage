// Copyright 2018-2018 the openage authors. See copying.md for legal info.

#include <cstdlib>
#include <epoxy/gl.h>
#include <functional>
#include <unordered_map>
#include <memory>
#include <eigen3/Eigen/Dense>

#include "../log/log.h"
#include "../error/error.h"
#include "../renderer/resources/shader_source.h"
#include "../renderer/resources/texture_data.h"
#include "../renderer/resources/mesh_data.h"
#include "../renderer/texture.h"
#include "../renderer/shader_program.h"
#include "../renderer/geometry.h"
#include "../renderer/opengl/window.h"


namespace openage::main::tests {

void engine_demo_0(const util::Path& path) {
	renderer::opengl::GlWindow window("openage engine test", 800, 600);

	auto renderer = window.make_renderer();

	auto vshader_src = renderer::resources::ShaderSource(
		renderer::resources::shader_lang_t::glsl,
		renderer::resources::shader_stage_t::vertex,
		R"s(
#version 330

layout(location=0) in vec2 position;
layout(location=1) in vec2 uv;
uniform mat4 mvp;
out vec2 v_uv;

void main() {
	gl_Position = mvp * vec4(position, 0.0, 1.0);
	v_uv = vec2(uv.x, 1.0 - uv.y);
}
)s");

	auto fshader_src = renderer::resources::ShaderSource(
		renderer::resources::shader_lang_t::glsl,
		renderer::resources::shader_stage_t::fragment,
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

	auto vshader_display_src = renderer::resources::ShaderSource(
		renderer::resources::shader_lang_t::glsl,
		renderer::resources::shader_stage_t::vertex,
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

	auto fshader_display_src = renderer::resources::ShaderSource(
		renderer::resources::shader_lang_t::glsl,
		renderer::resources::shader_stage_t::fragment,
		R"s(
#version 330

uniform sampler2D color_texture;

in vec2 v_uv;
out vec4 col;

void main() {
	col = texture(color_texture, v_uv);
}
)s");

	auto shader = renderer->add_shader( { vshader_src, fshader_src } );
	auto shader_display = renderer->add_shader( { vshader_display_src, fshader_display_src } );


	auto transform1 = Eigen::Affine3f::Identity();
	transform1.prescale(Eigen::Vector3f(0.4f, 0.2f, 1.0f));
	transform1.prerotate(Eigen::AngleAxisf(30.0f * 3.14159f / 180.0f, Eigen::Vector3f::UnitX()));
	transform1.pretranslate(Eigen::Vector3f(-0.4f, -0.6f, 0.0f));

	auto unif_in1 = shader->new_uniform_input(
		"mvp", transform1.matrix(),
		"u_id", 1u
	);

	auto transform2 = Eigen::Affine3f::Identity();
	transform2.prescale(Eigen::Vector3f(0.3f, 0.1f, 1.0f));
	transform2.prerotate(Eigen::AngleAxisf(50.0f * 3.14159f / 180.0f, Eigen::Vector3f::UnitZ()));

	auto transform3 = transform2;

	transform2.pretranslate(Eigen::Vector3f(0.3f, 0.1f, 0.3f));

	auto tex = renderer::resources::Texture2dData(path / "/assets/gaben.png");
	auto gltex = renderer->add_texture(tex);
	auto unif_in2 = shader->new_uniform_input(
		"mvp", transform2.matrix(),
		"u_id", 2u,
		"tex", gltex.get()
	);

	transform3.prerotate(Eigen::AngleAxisf(90.0f * 3.14159f / 180.0f, Eigen::Vector3f::UnitZ()));
	transform3.pretranslate(Eigen::Vector3f(0.3f, 0.1f, 0.5f));

	auto unif_in3 = shader->new_uniform_input(
		"mvp", transform3.matrix(),
		"u_id", 3u
	);

	auto quad = renderer->add_mesh_geometry(renderer::resources::MeshData::make_quad());
	renderer::Renderable obj1 {
		unif_in1.get(),
		quad.get(),
		true,
		true,
	};

	renderer::Renderable obj2{
		unif_in2.get(),
		quad.get(),
		true,
		true,
	};

	renderer::Renderable obj3 {
		unif_in3.get(),
		quad.get(),
		true,
		true,
	};

	auto size = window.get_size();
	auto color_texture = renderer->add_texture(renderer::resources::Texture2dInfo(size.first, size.second, renderer::resources::pixel_format::rgba8));
	auto id_texture = renderer->add_texture(renderer::resources::Texture2dInfo(size.first, size.second, renderer::resources::pixel_format::r32ui));
	auto depth_texture = renderer->add_texture(renderer::resources::Texture2dInfo(size.first, size.second, renderer::resources::pixel_format::depth24));
	auto fbo = renderer->create_texture_target( { color_texture.get(), id_texture.get(), depth_texture.get() } );

	auto color_texture_uniform = shader_display->new_uniform_input("color_texture", color_texture.get());
	renderer::Renderable display_obj {
		color_texture_uniform.get(),
		quad.get(),
		false,
		false,
	};

	renderer::RenderPass pass {
		{ obj1, obj2, obj3 },
		fbo.get()
	};

	renderer::RenderPass display_pass {
		{ display_obj },
		renderer->get_display_target(),
	};

	renderer::resources::Texture2dData id_texture_data = id_texture->into_data();
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
		} );

	window.add_resize_callback([&] (size_t w, size_t h) {
			// Calculate projection matrix
			float aspectRatio = float(w)/float(h);
			float xScale = 1.0/aspectRatio;

			Eigen::Matrix4f pmat;
			pmat << xScale, 0, 0, 0,
			             0, 1, 0, 0,
			             0, 0, 1, 0,
			             0, 0, 0, 1;

			// resize fbo
			color_texture = renderer->add_texture(renderer::resources::Texture2dInfo(w, h, renderer::resources::pixel_format::rgba8));
			id_texture = renderer->add_texture(renderer::resources::Texture2dInfo(w, h, renderer::resources::pixel_format::r32ui));
			depth_texture = renderer->add_texture(renderer::resources::Texture2dInfo(w, h, renderer::resources::pixel_format::depth24));
			fbo = renderer->create_texture_target( { color_texture.get(), id_texture.get(), depth_texture.get() } );
			texture_data_valid = false;

			shader_display->update_uniform_input(color_texture_uniform.get(), "color_texture", color_texture.get(), "proj", pmat);
			pass.target = fbo.get();
		} );

	while (!window.should_close()) {
		renderer->render(pass);
		renderer->render(display_pass);
		window.update();
		renderer::opengl::GlContext::check_error();
	}
}


void engine_demo(int demo_id, const util::Path &path) {
	switch (demo_id) {
	case 0:
		engine_demo_0(path);
		break;

	default:
		throw Error(ERR << "unknown engine demo " << demo_id << " requested.");
	}
}

} // namespace openage::main::tests
