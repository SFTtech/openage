// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include <cstdlib>
#include <epoxy/gl.h>
#include <functional>
#include <unordered_map>
#include <memory>

#include "../log/log.h"
#include "geometry.h"
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
uniform vec4 bounds;

void main() {
	gl_Position.x = 2.0 * float(gl_VertexID & 1) - 1.0;
	gl_Position.y = 2.0 * float((gl_VertexID & 2) >> 1) - 1.0;
	gl_Position.z = 1.0;
	gl_Position.w = 1.0;

	gl_Position.x *= bounds[2] - bounds[0];
	gl_Position.y *= bounds[3] - bounds[1];

	gl_Position.x += (bounds[0] + bounds[2]) / 2.0;
	gl_Position.y += (bounds[1] + bounds[3]) / 2.0;
}
)s");

	auto fshader_src = resources::ShaderSource(
		resources::shader_source_t::glsl_fragment,
		R"s(
#version 330

uniform vec4 color;
uniform uint u_id;
uniform uint writes_id;

layout(location=0) out vec4 col;
layout(location=1) out vec4 id;

void main() {
	col = color;
	uint id_write = u_id + 1u; // 0 is for no object
	vec3 id_vector = vec3(float(id_write >> 8) / 255.0, float(id_write & 255u) / 255.0, 0.0);
	if (bool(writes_id) && (color.a > 0.0)) {
		id = vec4(id_vector, 1.0);
	} else {
		id = vec4(id_vector, 0.0);
	}
}
)s");

	auto vshader_display_src = resources::ShaderSource(
		resources::shader_source_t::glsl_vertex,
		R"s(
#version 330

out vec2 v_uv;

void main() {
	gl_Position.x = 2.0 * float(gl_VertexID & 1) - 1.0;
	gl_Position.y = 2.0 * float((gl_VertexID & 2) >> 1) - 1.0;
	gl_Position.z = 1.0;
	gl_Position.w = 1.0;

	v_uv = gl_Position.xy * 0.5 + 0.5;
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
}
)s");

	auto shader = renderer->add_shader( { vshader_src, fshader_src } );
	auto shader_display = renderer->add_shader( { vshader_display_src, fshader_display_src } );

	auto unif_in1 = shader->new_uniform_input(
		"bounds", Eigen::Vector4f(-1.0f, -1.0f, -0.8f, -0.8f),
		"color", Eigen::Vector4f(1.0f, 0.0f, 0.0f, 1.0f),
		"u_id", 1u,
		"writes_id", 1u
	);

	auto unif_in2 = shader->new_uniform_input(
		"bounds", Eigen::Vector4f(0.0f, 0.3f, 0.3f, 0.5f),
		"color", Eigen::Vector4f(0.0f, 1.0f, 0.0f, 1.0f),
		"u_id", 2u,
		"writes_id", 1u
	);

	auto unif_in3 = shader->new_uniform_input(
		"bounds", Eigen::Vector4f(0.5f, -0.7f, 0.6f, -0.3f),
		"color", Eigen::Vector4f(0.0f, 0.0f, 1.0f, 0.5f),
		"u_id", 3u,
		"writes_id", 1u
	);

	Geometry quad;
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
	auto id_texture = renderer->add_texture(resources::TextureInfo(size.x, size.y, resources::pixel_format::rgba8));
	auto fbo = renderer->create_texture_target( { color_texture.get(), id_texture.get() } );

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
			id_texture = renderer->add_texture(resources::TextureInfo(new_size.x, new_size.y, resources::pixel_format::rgba8));
			fbo = renderer->create_texture_target( { color_texture.get(), id_texture.get() } );

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
			auto pixel_value = id_texture_data.read_pixel<uint32_t>(x, y);
			uint8_t *pixel_components = reinterpret_cast<uint8_t*>(&pixel_value);
			uint16_t id = (pixel_components[0] << 8) | pixel_components[1];
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
