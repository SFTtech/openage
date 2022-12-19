// Copyright 2015-2022 the openage authors. See copying.md for legal info.

#include "tests.h"

#include <QMouseEvent>
#include <cstdlib>
#include <eigen3/Eigen/Dense>
#include <epoxy/gl.h>
#include <functional>
#include <memory>
#include <unordered_map>

#include "error/error.h"
#include "log/log.h"
#include "renderer/geometry.h"
#include "renderer/gui/integration/public/gui_application_with_logger.h"
#include "renderer/opengl/window.h"
#include "renderer/resources/mesh_data.h"
#include "renderer/resources/shader_source.h"
#include "renderer/resources/texture_data.h"
#include "renderer/shader_program.h"
#include "renderer/texture.h"
#include "util/math_constants.h"

#include "renderer/camera/camera.h"
#include "renderer/entity_factory.h"
#include "renderer/stages/screen/screen_renderer.h"
#include "renderer/stages/skybox/skybox_renderer.h"
#include "renderer/stages/terrain/terrain_render_entity.h"
#include "renderer/stages/terrain/terrain_renderer.h"
#include "renderer/stages/world/world_render_entity.h"
#include "renderer/stages/world/world_renderer.h"


namespace openage::renderer::tests {

// Macro for debugging OpenGL state.
#define DBG_GL(txt) \
	printf("before %s\n", txt); \
	opengl::GlContext::check_error(); \
	printf("after %s\n", txt);

/**
 * Shows the renderer's ability to create textured renderable objects and
 * allow basic interaction with them via mouse/key callbacks.
 *
 * @param path Unused.
 */
void renderer_demo_0(const util::Path &path) {
	auto qtapp = std::make_shared<gui::GuiApplicationWithLogger>();

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

	window.add_mouse_button_callback([&](const QMouseEvent &ev) {
		auto qpos = ev.position();
		ssize_t x = qpos.x();
		ssize_t y = qpos.y();

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
		qtapp->process_events();

		renderer->check_error();
	}
	window.close();
}

/**
 * Basic test for creating a window and displaying a single-color texture
 * created by a shader.
 *
 * @param path Path to the project rootdir.
 */
void renderer_demo_2(const util::Path &path) {
	auto qtapp = std::make_shared<gui::GuiApplicationWithLogger>();

	opengl::GlWindow window("openage renderer test", 800, 600);
	auto renderer = window.make_renderer();

	auto display_vshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		R"s(
#version 330

layout(location=0) in vec2 position;

void main() {
	gl_Position = vec4(position, 0.0, 1.0);
}
)s");

	auto display_fshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		R"s(
#version 330

out vec4 col;

void main() {
    col = vec4(1.0, 0.4, 0.0, 0.8);
}
)s");

	auto display_shader = renderer->add_shader({display_vshader_src, display_fshader_src});

	auto quad = renderer->add_mesh_geometry(resources::MeshData::make_quad());
	Renderable display_stuff{
		display_shader->create_empty_input(),
		quad,
		false,
		false,
	};

	auto pass = renderer->add_render_pass({display_stuff}, renderer->get_display_target());

	while (not window.should_close()) {
		renderer->render(pass);
		window.update();
		qtapp->process_events();

		renderer->check_error();
	}
	window.close();
}


/**
 * Shows the different render stages and camera.
 *
 * @param path Path to the project rootdir.
 */
void renderer_demo_3(const util::Path &path) {
	auto qtapp = std::make_shared<gui::GuiApplicationWithLogger>();

	auto window = std::make_shared<opengl::GlWindow>("openage renderer test", 800, 600);
	auto renderer = window->make_renderer();

	// Camera
	auto camera = std::make_shared<renderer::camera::Camera>(window->get_size());
	window->add_resize_callback([&](size_t w, size_t h) {
		camera->resize(w, h);
	});

	// Render stages
	std::vector<std::shared_ptr<RenderPass>> render_passes{};
	auto skybox_renderer = std::make_shared<renderer::skybox::SkyboxRenderer>(
		window,
		renderer,
		path["assets"]["shaders"]);
	skybox_renderer->set_color(1.0f, 0.5f, 0.0f, 1.0f);

	auto terrain_renderer = std::make_shared<renderer::terrain::TerrainRenderer>(
		window,
		renderer,
		camera,
		path["assets"]["shaders"]);

	auto world_renderer = std::make_shared<renderer::world::WorldRenderer>(
		window,
		renderer,
		path["assets"]["shaders"]);

	render_passes.push_back(skybox_renderer->get_render_pass());
	render_passes.push_back(terrain_renderer->get_render_pass());
	render_passes.push_back(world_renderer->get_render_pass());

	// Final output on screen
	auto screen_renderer = std::make_shared<renderer::screen::ScreenRenderer>(
		window,
		renderer,
		path["assets"]["shaders"]);
	std::vector<std::shared_ptr<renderer::RenderTarget>> targets{};
	for (auto pass : render_passes) {
		targets.push_back(pass->get_target());
	}
	screen_renderer->set_render_targets(targets);

	render_passes.push_back(screen_renderer->get_render_pass());

	window->add_resize_callback([&](size_t, size_t) {
		std::vector<std::shared_ptr<renderer::RenderTarget>> targets{};
		for (size_t i = 0; i < render_passes.size() - 1; ++i) {
			targets.push_back(render_passes[i]->get_target());
		}
		screen_renderer->set_render_targets(targets);
	});

	// Create some entities to populate the scene
	auto render_factory = std::make_shared<RenderFactory>(terrain_renderer, world_renderer);

	// Terrain
	auto terrain0 = render_factory->add_terrain_render_entity();

	// fill a terrain grid with height values
	auto terrain_size = util::Vector2s{10, 10};
	std::vector<float> height_map{};
	height_map.reserve(terrain_size[0] * terrain_size[1]);
	for (size_t i = 0; i < terrain_size[0] * terrain_size[1]; ++i) {
		height_map.push_back(0.0f);
	}

	// Create "test bumps" in the terrain to check if rendering works
	height_map[11] = 1.0f;
	height_map[23] = 2.3f;
	height_map[42] = 4.2f;
	height_map[69] = 6.9f; // nice

	// A hill
	height_map[55] = 3.0f; // center
	height_map[45] = 2.0f; // bottom left slope
	height_map[35] = 1.0f;
	height_map[56] = 1.0f; // bottom right slope (little steeper)
	height_map[65] = 2.0f; // top right slope
	height_map[75] = 1.0f;
	height_map[54] = 2.0f; // top left slope
	height_map[53] = 1.0f;

	terrain0->update(terrain_size, height_map, path["assets"]["textures"]["test_terrain_tex.png"]);

	// Game entities
	auto world0 = render_factory->add_world_render_entity();
	world0->update(0, util::Vector3f(3.0f, 3.0f, 0.0f), path["assets"]["gaben.png"]);

	auto world1 = render_factory->add_world_render_entity();
	world1->update(1, util::Vector3f(7.5f, 6.0f, 0.0f), path["assets"]["missing.png"]);

	while (not window->should_close()) {
		qtapp->process_events();

		terrain_renderer->update();
		world_renderer->update();

		for (auto pass : render_passes) {
			renderer->render(pass);
		}

		renderer->check_error();

		window->update();
	}
	window->close();
}


void renderer_demo(int demo_id, const util::Path &path) {
	switch (demo_id) {
	case 0:
		renderer_demo_0(path);
		break;

	case 2:
		renderer_demo_2(path);
		break;

	case 3:
		renderer_demo_3(path);
		break;

	default:
		log::log(MSG(err) << "Unknown renderer demo requested: " << demo_id << ".");
		break;
	}
}

} // namespace openage::renderer::tests
