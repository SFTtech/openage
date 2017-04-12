// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "renderer.h"

#include "../../error/error.h"
#include "texture.h"
#include "shader_program.h"
#include "uniform_input.h"


namespace openage {
namespace renderer {
namespace opengl {

GlRenderer::GlRenderer(GlContext *ctx)
	: gl_context(ctx)
{
	log::log(MSG(info) << "Created OpenGL renderer");
}

std::unique_ptr<Texture> GlRenderer::add_texture(const resources::TextureData& data) {
	return std::make_unique<GlTexture>(data);
}

std::unique_ptr<ShaderProgram> GlRenderer::add_shader(std::vector<resources::ShaderSource> const& srcs) {
	return std::make_unique<GlShaderProgram>(srcs, this->gl_context->get_capabilities());
}

std::unique_ptr<RenderTarget> GlRenderer::create_texture_target(Texture const* tex) {
	return std::unique_ptr<RenderTarget>();
	//return std::make_unique<GlTextureTarget>(tex);
}

RenderTarget const* GlRenderer::get_framebuffer_target() {
	return this->framebuffer.get();
}

void GlRenderer::render(RenderPass const& pass) {
	for (auto obj : pass.renderables) {
		auto in = dynamic_cast<GlUniformInput const*>(obj.unif_in);
		in->program->execute_with(in, obj.geometry);
	}
}

}}}
