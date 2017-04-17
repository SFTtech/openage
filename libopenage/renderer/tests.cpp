// Copyright 2015-2015 the openage authors. See copying.md for legal info.

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
#include "renderable.h"


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
		switch (event.type) {
		case SDL_WINDOWEVENT:
			switch (event.window.event) {
			case SDL_WINDOWEVENT_RESIZED: {
				coord::window new_size{event.window.data1, event.window.data2};
				log::log(
					MSG(info) << "new window size: "
					<< new_size.x << " x " << new_size.y
				);
				//window.set_size(new_size);
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
		}
		case SDL_MOUSEBUTTONDOWN: {
			actions->click(event.button.x, event.button.y);
			break;
		}}
	}

	actions->frame();

    //window.swap();
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

	auto fshader_src = resources::ShaderSource(
		resources::shader_source_t::glsl_fragment,
		R"s(
#version 330

uniform vec4 color;
uniform uint u_id;
uniform uint writes_id;

layout(location=0) out vec4 col;
layout(location=1) out uint id;

void main() {
	col = color;
	if (bool(writes_id)) {
		id = u_id + 1u;
	} else {
		//not writing anything does produce undefined values
		id = 0u;
	}
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
	vec3 col2 = texture(color_texture, v_uv).xyz;
	col = vec4(col2, 1.0);
}
)s");


	std::vector<resources::ShaderSource> srcs = {vshader_src, fshader_src};
	std::vector<resources::ShaderSource> srcs_display = {vshader_display_src, fshader_display_src};
	auto shader = renderer->add_shader(srcs);
	log::log(INFO << "Before dispaly shader");
	auto shader_display = renderer->add_shader(srcs_display);

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
		"color", Eigen::Vector4f(0.0f, 0.0f, 1.0f, 1.0f),
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

	obj1.on_click_callback = [](){
		log::log(INFO << "Object 1 clicked");
	};
	shader->update_uniform_input(unif_in1.get(), "u_id", static_cast<uint32_t>(obj1.id));

	Renderable obj2{
		unif_in2.get(),
		&quad,
		true,
		true,
	};

	obj2.on_click_callback = [](){
		log::log(INFO << "Object 2 clicked");
	};
	shader->update_uniform_input(unif_in2.get(), "u_id", static_cast<uint32_t>(obj2.id));

	Renderable obj3 {
		unif_in3.get(),
		&quad,
		true,
		true,
		false
	};

	obj3.on_click_callback = [](){
		log::log(INFO << "Object 3 clicked");
	};
	shader->update_uniform_input(unif_in3.get(), "u_id", static_cast<uint32_t>(obj3.id), "writes_id", obj3.writes_id ? 1u : 0u);

	std::unique_ptr<opengl::GlTexture> color_texture = std::unique_ptr<opengl::GlTexture>( new opengl::GlTexture(window.get_size().x, window.get_size().y, resources::pixel_format::rgb8) );
	std::unique_ptr<opengl::GlTexture> id_texture = std::unique_ptr<opengl::GlTexture>( new opengl::GlTexture(window.get_size().x, window.get_size().y, resources::pixel_format::r16ui) );
	opengl::GlTexture* color_texture_ptr = color_texture.get();
	opengl::GlTexture* id_texture_ptr = id_texture.get();
	std::vector<const opengl::GlTexture*> fbo_textures;
	fbo_textures.push_back(color_texture_ptr);
	fbo_textures.push_back(id_texture_ptr);

	auto fbo = std::unique_ptr<opengl::GlRenderTarget>(new opengl::GlRenderTarget(fbo_textures));

	auto color_texture_uniform = shader_display->new_uniform_input("color_texture", static_cast<Texture const*>(color_texture.get()));
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
		{ display_obj } ,
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

			//GLuint vpos_buf;
			//glGenBuffers(1, &vpos_buf);
			//glBindBuffer(GL_ARRAY_BUFFER, vpos_buf);

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
			/*simplequad->use();

			glBindBuffer(GL_ARRAY_BUFFER, vpos_buf);
			glEnableVertexAttribArray(posattr_id);
			glVertexAttribPointer(posattr_id, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glDisableVertexAttribArray(posattr_id);

			renderer.check_error();*/
		},
		// resize
		[&](const coord::window &new_size) {
			// handle in renderer..
			glViewport(0, 0, new_size.x, new_size.y);
			//resize fbo
			color_texture = std::unique_ptr<opengl::GlTexture>( new opengl::GlTexture(new_size.x, new_size.y, resources::pixel_format::rgb8) );
			id_texture = std::unique_ptr<opengl::GlTexture>( new opengl::GlTexture(new_size.x, new_size.y, resources::pixel_format::r16ui) );
			std::vector<const opengl::GlTexture*> fbo_textures_new;
			fbo_textures_new.push_back(color_texture.get());
			fbo_textures_new.push_back(id_texture.get());

			fbo = std::unique_ptr<opengl::GlRenderTarget>(new opengl::GlRenderTarget(fbo_textures_new));
			shader_display->update_uniform_input(color_texture_uniform.get(), "color_texture", static_cast<Texture const*>(color_texture.get()));
			pass.target = fbo.get();
		},
		//click
		[&](const uint16_t x, const uint16_t y) {
			log::log(INFO << "Clicked at location (" << x << ", " << y << ")");
			if (!texture_data_valid) {
				id_texture_data = id_texture->into_data();
				texture_data_valid = true;
			}
			auto texture_dimensions = id_texture->get_info().get_size();
			auto position = (texture_dimensions.second-y)*texture_dimensions.first + x;
			position *= 2; //2 byte per pixel
			auto id = *reinterpret_cast<const uint16_t*>(id_texture_data.get_data()+position);
			log::log(INFO << "Id-texture-value at location: " << id);
			if (id == 0) {
				//no renderable at given location
				log::log(INFO << "Clicked at non existent object");
				return;
			}
			id--; //real id is id-1
			Renderable::get(id)->on_click_callback();
		}
	};

	render_test(window, &test0);
}


	/*void renderer_demo_1() {
	Window window{"openage renderer testing"};
	Renderer renderer{window.get_context()};
	// instead do
	// Renderer renderer(&window);
	// also do
	// GuiRenderer gui(&window) using qtquick
	// probably use shared_ptr for window so as to guarantee lifetimes

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
		[&](Window * /*window*//*) {
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
}*/

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
