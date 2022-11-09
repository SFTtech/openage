// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#include "terrain_renderer.h"

#include "renderer/opengl/context.h"
#include "renderer/renderer.h"
#include "renderer/resources/texture_info.h"
#include "renderer/window.h"

namespace openage::renderer::terrain {

TerrainRenderer::TerrainRenderer(const std::shared_ptr<Window> &window,
                                 const std::shared_ptr<renderer::Renderer> &renderer,
                                 const util::Path &shaderdir) :
	renderer{renderer} {
	renderer::opengl::GlContext::check_error();

	this->initialize_render_pass(shaderdir);

	window->add_resize_callback([this](size_t width, size_t height) {
		this->resize(width, height);
	});
}

std::shared_ptr<renderer::RenderPass> TerrainRenderer::get_render_pass() {
	return this->render_pass;
}

void TerrainRenderer::update() {
	// TODO
}

void TerrainRenderer::resize(size_t width, size_t height) {
	auto color_texture = renderer->add_texture(
		resources::Texture2dInfo(width, height, resources::pixel_format::rgba8));
	auto id_texture = renderer->add_texture(
		resources::Texture2dInfo(width, height, resources::pixel_format::r32ui));
	auto depth_texture = renderer->add_texture(
		resources::Texture2dInfo(width, height, resources::pixel_format::depth24));
	auto fbo = this->renderer->create_texture_target({color_texture, id_texture, depth_texture});

	this->render_pass->set_target(fbo);
}

void TerrainRenderer::initialize_render_pass(const util::Path &shaderdir) {
	// TODO: Load shaders, create renderables
	auto fbo = this->renderer->create_texture_target({});
	this->render_pass = this->renderer->add_render_pass({}, fbo);
}

} // namespace openage::renderer::terrain