// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "hud_renderer.h"

#include "renderer/camera/camera.h"
#include "renderer/opengl/context.h"
#include "renderer/resources/assets/asset_manager.h"
#include "renderer/resources/shader_source.h"
#include "renderer/resources/texture_info.h"
#include "renderer/shader_program.h"
#include "renderer/stages/hud/hud_object.h"
#include "renderer/texture.h"
#include "renderer/window.h"
#include "time/clock.h"


namespace openage::renderer::hud {

HudRenderer::HudRenderer(const std::shared_ptr<Window> &window,
                         const std::shared_ptr<renderer::Renderer> &renderer,
                         const std::shared_ptr<renderer::camera::Camera> &camera,
                         const util::Path &shaderdir,
                         const std::shared_ptr<renderer::resources::AssetManager> &asset_manager,
                         const std::shared_ptr<time::Clock> clock) :
	renderer{renderer},
	camera{camera},
	asset_manager{asset_manager},
	render_objects{},
	clock{clock} {
	renderer::opengl::GlContext::check_error();

	auto size = window->get_size();
	this->initialize_render_pass(size[0], size[1], shaderdir);

	window->add_resize_callback([this](size_t width, size_t height, double /*scale*/) {
		this->resize(width, height);
	});

	log::log(INFO << "Created render stage 'HUD'");
}

std::shared_ptr<renderer::RenderPass> HudRenderer::get_render_pass() {
	return this->render_pass;
}

void HudRenderer::add_render_entity(const std::shared_ptr<HudRenderEntity> entity) {
	std::unique_lock lock{this->mutex};

	auto hud_object = std::make_shared<HudObject>(this->asset_manager);
	hud_object->set_render_entity(entity);
	hud_object->set_camera(this->camera);
	this->render_objects.push_back(hud_object);
}

void HudRenderer::update() {
	std::unique_lock lock{this->mutex};
	auto current_time = this->clock->get_real_time();
	for (auto &obj : this->render_objects) {
		// TODO
	}
}

void HudRenderer::resize(size_t width, size_t height) {
	this->output_texture = renderer->add_texture(resources::Texture2dInfo(width, height, resources::pixel_format::rgba8));
	this->depth_texture = renderer->add_texture(resources::Texture2dInfo(width, height, resources::pixel_format::depth24));

	auto fbo = this->renderer->create_texture_target({this->output_texture, this->depth_texture});
	this->render_pass->set_target(fbo);
}

void HudRenderer::initialize_render_pass(size_t width,
                                         size_t height,
                                         const util::Path &shaderdir) {
	// ASDF: add vertex shader
	auto vert_shader_file = (shaderdir / "world2d.vert.glsl").open();
	auto vert_shader_src = renderer::resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		vert_shader_file.read());
	vert_shader_file.close();

	// ASDF: add fragment shader
	auto frag_shader_file = (shaderdir / "world2d.frag.glsl").open();
	auto frag_shader_src = renderer::resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		frag_shader_file.read());
	frag_shader_file.close();

	this->output_texture = renderer->add_texture(resources::Texture2dInfo(width, height, resources::pixel_format::rgba8));
	this->depth_texture = renderer->add_texture(resources::Texture2dInfo(width, height, resources::pixel_format::depth24));

	this->display_shader = this->renderer->add_shader({vert_shader_src, frag_shader_src});

	auto fbo = this->renderer->create_texture_target({this->output_texture, this->depth_texture});
	this->render_pass = this->renderer->add_render_pass({}, fbo);
}

} // namespace openage::renderer::hud
