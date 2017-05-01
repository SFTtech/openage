// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include <cstdlib>
#include <epoxy/gl.h>
#include <functional>
#include <unordered_map>
#include <memory>
#include <eigen3/Eigen/Dense>

#include "../log/log.h"
#include "opengl/geometry.h"
#include "../error/error.h"
#include "resources/shader_source.h"
#include "opengl/renderer.h"
#include "window.h"


namespace openage {
namespace renderer {
namespace tests {

/**
 * render demo function collection.
 */
struct render_demo {
	std::function<void(Window *)> setup;
	std::function<void()> frame;
	std::function<void(const coord::window &)> resize;
	std::function<void(const uint16_t, const uint16_t)> click;
};

void render_test(Window &window, const render_demo *actions) {
	SDL_Event event;

	actions->setup(&window);

	bool running = true;
	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
				coord::window new_size{event.window.data1, event.window.data2};
				log::log(
					MSG(info) << "new window size: "
					<< new_size.x << " x " << new_size.y
				);
				actions->resize(new_size);
			}
			else if (event.type == SDL_QUIT) {
				running = false;
			}
			else if (event.type == SDL_KEYUP) {
				SDL_Keycode sym = reinterpret_cast<SDL_KeyboardEvent *>(&event)->keysym.sym;
				if (sym == SDLK_ESCAPE) {
					running = false;
				}
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN) {
				actions->click(event.button.x, event.button.y);
			}
		}

		actions->frame();
	}
}

void renderer_demo_0() {
	Window window { "openage renderer testing" };
	window.make_context_current();
	auto renderer = std::make_unique<opengl::GlRenderer>(window.get_context());

	auto vshader_src = resources::ShaderSource(
		resources::shader_source_t::glsl_vertex,
		R"s(
#version 330
layout(location=0) in vec2 in_position;

uniform mat4 mvp;

void main() {
	gl_Position = mvp * vec4(in_position, 0.0, 1.0);
}
)s");

	auto fshader_src = resources::ShaderSource(
		resources::shader_source_t::glsl_fragment,
		R"s(
#version 330

uniform vec4 color;
uniform uint u_id;

layout(location=0) out vec4 col;
layout(location=1) out uint id;

void main() {
	if (color.a == 0.0) {
		discard;
	}
	col = color;
	id = u_id + 1u;
}
)s");

	auto vshader_display_src = resources::ShaderSource(
		resources::shader_source_t::glsl_vertex,
		R"s(
#version 330

layout(location=0) in vec2 in_position;
layout(location=1) in vec2 in_texcoord;
out vec2 v_uv;

void main() {
	gl_Position = vec4(in_position, 0.0, 1.0);

	v_uv = in_texcoord;
}
)s");


	auto fshader_display_src = resources::ShaderSource(
		resources::shader_source_t::glsl_fragment,
		R"s(
#version 330
uniform sampler2D color_texture;

in vec2 v_uv;
out vec4 col;

void main() {
	col = texture(color_texture, v_uv);
	//col = vec4(v_uv, 0.0, 1.0);
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
		"color", Eigen::Vector4f(1.0f, 0.0f, 0.0f, 1.0f),
		"u_id", 1u
	);

	auto transform2 = Eigen::Affine3f::Identity();
	transform2.prescale(Eigen::Vector3f(0.3f, 0.1f, 1.0f));
	transform2.prerotate(Eigen::AngleAxisf(50.0f * 3.14159f / 180.0f, Eigen::Vector3f::UnitZ()));

	auto transform3 = transform2;

	transform2.pretranslate(Eigen::Vector3f(0.3f, 0.1f, 0.3f));

	auto unif_in2 = shader->new_uniform_input(
		"mvp", transform2.matrix(),
		"color", Eigen::Vector4f(0.0f, 1.0f, 0.0f, 1.0f),
		"u_id", 2u
	);

	transform3.prerotate(Eigen::AngleAxisf(90.0f * 3.14159f / 180.0f, Eigen::Vector3f::UnitZ()));
	transform3.pretranslate(Eigen::Vector3f(0.3f, 0.1f, 0.5f));

	auto unif_in3 = shader->new_uniform_input(
		"mvp", transform3.matrix(),
		"color", Eigen::Vector4f(0.0f, 0.0f, 1.0f, 1.0f),
		"u_id", 3u
	);

	opengl::GlGeometry quad(geometry_t::quad);
	Renderable obj1 {
		unif_in1.get(),
		&quad,
		true,
		true,
	};

	Renderable obj2{
		unif_in2.get(),
		&quad,
		true,
		true,
	};

	Renderable obj3 {
		unif_in3.get(),
		&quad,
		true,
		true,
	};

	auto size = window.get_size();
	auto color_texture = renderer->add_texture(resources::TextureInfo(size.x, size.y, resources::pixel_format::rgba8));
	auto id_texture = renderer->add_texture(resources::TextureInfo(size.x, size.y, resources::pixel_format::rgba8ui));
	auto depth_texture = renderer->add_texture(resources::TextureInfo(size.x, size.y, resources::pixel_format::depth24));
	auto fbo = renderer->create_texture_target( { color_texture.get(), id_texture.get(), depth_texture.get() } );

	auto color_texture_uniform = shader_display->new_uniform_input("color_texture", color_texture.get());
	Renderable display_obj {
		color_texture_uniform.get(),
		&quad,
		false,
		false,
	};

	RenderPass pass {
		{ obj1, obj2, obj3 },
		fbo.get()
	};

	RenderPass display_pass {
		{ display_obj },
		renderer->get_display_target(),
	};

	resources::TextureData id_texture_data = id_texture->into_data();
	bool texture_data_valid = false;

	render_demo test0{
		// init
		[&](Window */*window*/) {
			glDepthFunc(GL_LEQUAL);
			glDepthRange(0.0, 1.0);
			// what is this
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			// TODO put in renderer
			GLuint vao;
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao); // stores all the vertex attrib state.*/
		},
		// frame
		[&]() {
			texture_data_valid = false;
			renderer->render(pass);
			renderer->render(display_pass);
			window.swap();
			window.get_context()->check_error();
		},
		// resize
		[&](const coord::window &new_size) {
			// handle in renderer..
			glViewport(0, 0, new_size.x, new_size.y);

			// resize fbo
			color_texture = renderer->add_texture(resources::TextureInfo(new_size.x, new_size.y, resources::pixel_format::rgba8));
			id_texture = renderer->add_texture(resources::TextureInfo(new_size.x, new_size.y, resources::pixel_format::rgba8ui));
			depth_texture = renderer->add_texture(resources::TextureInfo(new_size.x, new_size.y, resources::pixel_format::depth24));
			fbo = renderer->create_texture_target( { color_texture.get(), id_texture.get(), depth_texture.get() } );

			shader_display->update_uniform_input(color_texture_uniform.get(), "color_texture", color_texture.get());
			pass.target = fbo.get();
		},
		//click
		[&](const uint16_t x, const uint16_t y) {
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
		}
	};

	render_test(window, &test0);
}

void renderer_demo(int demo_id) {
	switch (demo_id) {
	case 0:
		renderer_demo_0();
		break;

	default:
		log::log(MSG(err) << "unknown renderer demo " << demo_id << " requested.");
		break;
	}
}

}}} // namespace openage::renderer::tests
