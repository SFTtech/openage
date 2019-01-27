// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#include "tests.h"

#include <cstdlib>
#include <epoxy/gl.h>
#include <functional>
#include <unordered_map>
#include <memory>
#include <eigen3/Eigen/Dense>

#include "../log/log.h"
#include "../error/error.h"
#include "../util/math_constants.h"
#include "resources/shader_source.h"
#include "resources/texture_data.h"
#include "resources/mesh_data.h"
#include "texture.h"
#include "shader_program.h"
#include "geometry.h"
#include "opengl/window.h"


namespace openage {
namespace renderer {
namespace tests {

// Macro for debugging OpenGL state.
#define DBG_GL(txt)                    \
	printf("before %s\n", txt);          \
	opengl::GlContext::check_error();    \
	printf("after %s\n", txt);

void renderer_demo_0(const util::Path& path) {
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

	auto obj_shader = renderer->add_shader( { obj_vshader_src, obj_fshader_src } );
	auto display_shader = renderer->add_shader( { display_vshader_src, display_fshader_src } );

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
		"mv", transform1.matrix(),
		"u_id", 1u,
		"tex", gltex
	);

	auto transform2 = Eigen::Affine3f::Identity();
	transform2.prescale(Eigen::Vector3f(0.3f, 0.1f, 1.0f));
	transform2.prerotate(Eigen::AngleAxisf(50.0f * math::PI / 180.0f, Eigen::Vector3f::UnitZ()));

	auto transform3 = transform2;

	transform2.pretranslate(Eigen::Vector3f(0.3f, 0.1f, 0.3f));

	auto obj2_unifs = obj_shader->new_uniform_input(
		"mv", transform2.matrix(),
		"u_id", 2u,
		// TODO bug: this tex input spills over to all the other uniform inputs!
		"tex", gltex
	);

	transform3.prerotate(Eigen::AngleAxisf(90.0f * math::PI / 180.0f, Eigen::Vector3f::UnitZ()));
	transform3.pretranslate(Eigen::Vector3f(0.3f, 0.1f, 0.5f));

	auto obj3_unifs = obj_shader->new_uniform_input(
		"mv", transform3.matrix(),
		"u_id", 3u,
		"tex", gltex
	);

	/* The objects are using built-in quadrilateral geometry. */
	auto quad = renderer->add_mesh_geometry(resources::MeshData::make_quad());
	Renderable obj1 {
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

	Renderable obj3 {
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
	auto fbo = renderer->create_texture_target( { color_texture, id_texture, depth_texture } );

	/* Make an object to update the projection matrix in pass 1 according to changes in the screen size.
	 * Because uniform values are preserved across objects using the same shader in a single render pass,
	 * it is sufficient to set it once at the beginning of the pass. To do this, we use an object with no
	 * geometry, which will set the uniform but not render anything. */
	auto proj_unif = obj_shader->new_uniform_input();
	Renderable proj_update {
		proj_unif,
		nullptr,
		false,
		false,
	};

	auto pass1 = renderer->add_render_pass(
		{ proj_update, obj1, obj2, obj3 },
		fbo
	);

	/* Make an object encompassing the entire screen for the second render pass. The object
	 * will be textured with the color output of pass 1, effectively copying its framebuffer. */
	auto color_texture_unif = display_shader->new_uniform_input("color_texture", color_texture);
	Renderable display_obj {
		color_texture_unif,
		quad,
		false,
		false,
	};

	auto pass2 = renderer->add_render_pass({ display_obj }, renderer->get_display_target());

	/* Data retrieved from the object index texture. */
	resources::Texture2dData id_texture_data = id_texture->into_data();
	bool texture_data_valid = false;

	// TODO(Vtec234): move these into the renderer
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0, 1.0);
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
			log::log(INFO << "Clicked at non existent object");
		} else {
			log::log(INFO << "Object number " << id << " clicked.");
		}
	});

	window.add_resize_callback([&] (size_t w, size_t h) {
		/* Calculate a projection matrix for the new screen size. */
		float aspectRatio = float(w)/float(h);
		float xScale = 1.0/aspectRatio;

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
		fbo = renderer->create_texture_target( { color_texture, id_texture, depth_texture } );

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

void renderer_demo(int demo_id, const util::Path &path) {
	switch (demo_id) {
	case 0:
		renderer_demo_0(path);
		break;

	default:
		log::log(MSG(err) << "Unknown renderer demo requested: " << demo_id << ".");
		break;
	}
}

}}} // namespace openage::renderer::tests
