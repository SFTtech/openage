// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include <cstdlib>
#include <epoxy/gl.h>
#include <functional>
#include <unordered_map>

#include "../log/log.h"
#include "../util/opengl.h"
#include "opengl/shader.h"
#include "opengl/program.h"
#include "window.h"
#include "renderer.h"
#include "shader.h"

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
};


void render_test(Window &window, const render_demo *actions) {
	SDL_Event event;

	actions->setup(&window);

	bool running = true;
	while (running) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_WINDOWEVENT:
				switch (event.window.event) {
				case SDL_WINDOWEVENT_RESIZED: {
					coord::window new_size{event.window.data1, event.window.data2};
					log::log(
						MSG(info) << "new window size: "
						<< new_size.x << " x " << new_size.y
					);
					window.set_size(new_size);
					actions->resize(new_size);
					break;
				}}
				break;

			case SDL_QUIT:
				running = false;
				break;

			case SDL_KEYUP: {
				SDL_Keycode sym = reinterpret_cast<SDL_KeyboardEvent *>(&event)->keysym.sym;
				switch (sym) {
				case SDLK_ESCAPE:
					running = false;
					break;
				default:
					break;
				}
				break;
			}}
		}

		actions->frame();

		window.swap();
	}
}


void renderer_demo() {
	int demo_id = 0;

	Window window{"openage renderer testing"};
	Renderer renderer{window.get_context()};

	ShaderSourceCode vshader_src(
		shader_type::vertex,
		"#version 330\n"
		"layout(location = 0) in vec4 position;"
		"smooth out vec4 fragpos;"
		"void main() {"
		"fragpos = position;"
		"gl_Position = position;"
		"}"
	);

	ShaderSourceCode fshader_src(
		shader_type::fragment,
		"#version 330\n"
		"out vec4 color;\n"
		"smooth in vec4 fragpos;"
		"void main() {"
		"color = vec4(1.0f, fragpos.y, fragpos.x, 1.0f);"
		"}"
	);

	ProgramSource simplequad_src({&vshader_src, &fshader_src});

	std::shared_ptr<Program> simplequad = renderer.add_program(simplequad_src);

	simplequad->dump_attributes();

	const float vpos[] = {
		-1.0f, -1.0f, .0f, 1.0f,
		1.0f, -1.0f, .0f, 1.0f,
		-1.0f, 1.0f, .0f, 1.0f,

		1.0f, -1.0f, .0f, 1.0f,
		-1.0f, 1.0f, .0f, 1.0f,
		1.0f, 1.0f, .0f, 1.0f,
	};

	GLuint vpos_buf, posattr_id = 0;

	//posattr_id = simplequad->get_attribute_id("position");

	GLuint vao;

	render_demo test0{
		// init
		[&](Window */*window*/) {
			glEnable(GL_BLEND);

			glGenBuffers(1, &vpos_buf);
			glBindBuffer(GL_ARRAY_BUFFER, vpos_buf);
			// save vertex attributes to GPU:
			glBufferData(GL_ARRAY_BUFFER, sizeof(vpos), vpos, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao); // stores all the vertex attrib state.
		},
		// frame
		[&]() {
			glClearColor(0.0, 0.0, 0.2, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			simplequad->use();

			glBindBuffer(GL_ARRAY_BUFFER, vpos_buf);
			glEnableVertexAttribArray(posattr_id);
			glVertexAttribPointer(posattr_id, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glDisableVertexAttribArray(posattr_id);

			util::gl_check_error();
		},
		// resize
		[&](const coord::window &new_size) {
			renderer.on_resize(new_size);
		}
	};

	std::unordered_map<int, render_demo*> demos;
	demos[0] = &test0;

	if (demos.find(demo_id) != demos.end()) {
		render_test(window, demos[demo_id]);
	}
	else {
		log::log(MSG(err) << "unknown renderer demo " << demo_id << " requested.");
	}
}


}}} // namespace openage::renderer::tests
