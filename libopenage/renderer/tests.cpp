// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include <cstdlib>
#include <epoxy/gl.h>
#include <functional>
#include <unordered_map>

#include "../log/log.h"
#include "opengl/shader.h"
#include "opengl/program.h"
#include "vertex_state.h"
#include "window.h"
#include "renderer.h"
#include "shader.h"
#include "shaders/simpletexture.h"
#include "texture.h"

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

void renderer_demo_0() {
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

	std::unique_ptr<Program> simplequad = renderer.add_program(simplequad_src);

	simplequad->dump_attributes();

	float val = 0.9f;
	const float vpos[] = {
		-val, -val, .0f, 1.0f,
		val, -val, .0f, 1.0f,
		-val, val, .0f, 1.0f,

		val, -val, .0f, 1.0f,
		-val, val, .0f, 1.0f,
		val, val, .0f, 1.0f,
	};

	GLuint vpos_buf, posattr_id = 0;

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

			renderer.check_error();
		},
		// resize
		[&](const coord::window &new_size) {
			renderer.on_resize(new_size);
		}
	};

	render_test(window, &test0);
}


void renderer_demo_1() {
	Window window{"openage renderer testing"};
	Renderer renderer{window.get_context()};

	ShaderSourceCode vshader_src(
		shader_type::vertex,
		"#version 330\n"
		"layout(location = 0) in vec4 position;"
		"layout(location = 1) in vec2 texcoord;"
		"smooth out vec2 texpos;"
		"void main() {"
		"texpos = texcoord;"
		"gl_Position = position;"
		"}"
	);

	ShaderSourceCode fshader_src(
		shader_type::fragment,
		"#version 330\n"
		"out vec4 color;"
		"smooth in vec2 texpos;"
		"uniform sampler2D tex;"
		"void main() {"
		"color = texture(tex, texpos.xy);"
		"}"
	);

	ProgramSource simpletex_src({&vshader_src, &fshader_src});
	std::unique_ptr<Program> simpletex = renderer.add_program(simpletex_src);
	simpletex->dump_attributes();

	SimpleTextureMaterial tex_pipeline{simpletex.get()};

	VertexBuffer vbo{window.get_context().get()};

	FileTextureData gaben_data{"assets/gaben.png"};
	std::unique_ptr<Texture> gaben = renderer.add_texture(gaben_data);
	std::unique_ptr<VertexState> vao = window.get_context()->create_vertex_state();

	render_demo test1{
		// init
		[&](Window */*window*/) {
			log::log(MSG(dbg) << "preparing test");

			tex_pipeline.tex.set(gaben.get());
			tex_pipeline.position.set_layout(0);
			tex_pipeline.texcoord.set_layout(1);

			float val = 0.9f;
			tex_pipeline.set_positions({
				{-val, -val, .0f, 1.0f},
				{val, -val, .0f, 1.0f},
				{-val, val, .0f, 1.0f},

				{val, -val, .0f, 1.0f},
				{-val, val, .0f, 1.0f},
				{val, val, .0f, 1.0f},
			});

			tex_pipeline.texcoord.set({
				{0.0f, 1.0f},
				{1.0f, 1.0f},
				{0.0f, 0.0f},

				{1.0f, 1.0f},
				{0.0f, 0.0f},
				{1.0f, 0.0f},
			});

			// apply the pipeline properties
			tex_pipeline.upload_uniforms();
			tex_pipeline.update_buffer(&vbo);
			vao->attach_buffer(vbo); // upload buffer

			vao->bind();
		},
		// frame
		[&]() {
			glClearColor(0.0, 0.0, 0.2, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			// combination of geometry and material with translations
			// = draw task.

			// geometry: manager owns.
			// material: manager owns?

			//Task t = material.draw(geometry);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			renderer.check_error();
		},
		// resize
		[&](const coord::window &new_size) {
			renderer.on_resize(new_size);
		}
	};

	render_test(window, &test1);
}


void renderer_demo(int demo_id) {
	switch (demo_id) {
	case 0:
		renderer_demo_0();
		break;

	case 1:
		renderer_demo_1();
		break;

	default:
		log::log(MSG(err) << "unknown renderer demo " << demo_id << " requested.");
		break;
	}
}


}}} // namespace openage::renderer::tests
