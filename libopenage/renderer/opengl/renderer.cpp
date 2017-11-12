// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "renderer.h"

#include "../../log/log.h"
#include "../../error/error.h"
#include "texture.h"
#include "shader_program.h"
#include "uniform_input.h"
#include "geometry.h"


namespace openage {
namespace renderer {
namespace opengl {

GlRenderer::GlRenderer(GlContext *ctx)
	: gl_context(ctx)
	, display()
{
	log::log(MSG(info) << "Created OpenGL renderer");
}

std::unique_ptr<Texture> GlRenderer::add_texture(const resources::TextureData& data) {
	return std::make_unique<GlTexture>(data);
}

std::unique_ptr<Texture> GlRenderer::add_texture(const resources::TextureInfo& info) {
	return std::make_unique<GlTexture>(info);
}

std::unique_ptr<ShaderProgram> GlRenderer::add_shader(std::vector<resources::ShaderSource> const& srcs) {
	return std::make_unique<GlShaderProgram>(srcs, this->gl_context->get_capabilities());
}

std::unique_ptr<Geometry> GlRenderer::add_mesh_geometry(resources::MeshData const& mesh) {
	return std::make_unique<GlGeometry>(mesh);
}

std::unique_ptr<Geometry> GlRenderer::add_bufferless_quad() {
	return std::make_unique<GlGeometry>();
}

std::unique_ptr<RenderTarget> GlRenderer::create_texture_target(std::vector<Texture*> textures) {
	std::vector<const GlTexture*> gl_textures;
	for (auto tex : textures) {
		gl_textures.push_back(static_cast<const GlTexture*>(tex));
	}

	return std::make_unique<GlRenderTarget>(gl_textures);
}

RenderTarget const* GlRenderer::get_display_target() {
	return &this->display;
}

void GlRenderer::render(RenderPass const& pass) {
	auto gl_target = dynamic_cast<const GlRenderTarget*>(pass.target);
	gl_target->bind_write();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto obj : pass.renderables) {
		if (obj.alpha_blending) {
			glEnable(GL_BLEND);
		}
		else {
			glDisable(GL_BLEND);
		}

		if (obj.depth_test) {
			glEnable(GL_DEPTH_TEST);
		}
		else {
			glDisable(GL_DEPTH_TEST);
		}

		auto in = dynamic_cast<GlUniformInput const*>(obj.unif_in);
		auto geom = dynamic_cast<GlGeometry const*>(obj.geometry);
		in->program->execute_with(in, geom);
	}
}

}}} // openage::renderer::opengl
