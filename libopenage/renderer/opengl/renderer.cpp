// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "renderer.h"

#include "renderable.h"
#include "../../error/error.h"


namespace openage {
namespace renderer {
namespace opengl {

GlRenderer::GlRenderer(GlContext *ctx)
	: gl_context(ctx)
{
	log::log(MSG(info) << "Created OpenGL renderer");
}

std::unique_ptr<Texture> add_texture(resources::TextureData const&) {

}

std::unique_ptr<ShaderProgram> add_shader(std::vector<resources::ShaderSource> const& srcs) {
	return std::make_unique<GlShaderProgram>(srcs);
}

std::unique_ptr<RenderTarget> create_texture_target(Texture const*) {

}

RenderTarget const* get_framebuffer_target() {
	return this->framebuffer.get();
}

void render(RenderPass const& pass) {
	for (auto obj : pass.renderables) {
		this->shaders[0].execute_with(*dynamic_cast<GlRenderable*>(obj.get()));
	}
}


Renderer::ShaderId GlRenderer::add_shader(const std::vector<resources::ShaderSource>& srcs) {
}

bool GlRenderer::remove_shader(Renderer::ShaderId id) {

}

}}}
