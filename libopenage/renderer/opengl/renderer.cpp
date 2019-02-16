// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#include "renderer.h"

#include <algorithm>

#include "../../log/log.h"
#include "../../error/error.h"
#include "texture.h"
#include "shader_program.h"
#include "uniform_input.h"
#include "geometry.h"


namespace openage::renderer::opengl {

GlRenderer::GlRenderer(GlContext *ctx)
	:
	gl_context(ctx),
	display{std::make_shared<GlRenderTarget>()}
{
	log::log(MSG(info) << "Created OpenGL renderer");
}

std::shared_ptr<Texture2d> GlRenderer::add_texture(const resources::Texture2dData& data) {
	return std::make_shared<GlTexture2d>(data);
}

std::shared_ptr<Texture2d> GlRenderer::add_texture(const resources::Texture2dInfo& info) {
	return std::make_shared<GlTexture2d>(info);
}

std::shared_ptr<ShaderProgram> GlRenderer::add_shader(std::vector<resources::ShaderSource> const& srcs) {
	return std::make_shared<GlShaderProgram>(srcs, this->gl_context->get_capabilities());
}

std::shared_ptr<Geometry> GlRenderer::add_mesh_geometry(resources::MeshData const& mesh) {
	return std::make_shared<GlGeometry>(mesh);
}

std::shared_ptr<Geometry> GlRenderer::add_bufferless_quad() {
	return std::make_shared<GlGeometry>();
}

std::shared_ptr<RenderPass> GlRenderer::add_render_pass(std::vector<Renderable> renderables, const std::shared_ptr<RenderTarget> &target) {
	return std::make_shared<GlRenderPass>(std::move(renderables), target);
}

std::shared_ptr<RenderTarget> GlRenderer::create_texture_target(std::vector<std::shared_ptr<Texture2d>> const& textures) {
	std::vector<std::shared_ptr<GlTexture2d>> gl_textures;
	gl_textures.reserve(textures.size());
	for (auto tex : textures) {
		gl_textures.push_back(std::dynamic_pointer_cast<GlTexture2d>(tex));
	}

	return std::make_shared<GlRenderTarget>(gl_textures);
}

std::shared_ptr<RenderTarget> GlRenderer::get_display_target() {
	return this->display;
}

resources::Texture2dData GlRenderer::display_into_data() {
	GLint params[4];
	glGetIntegerv(GL_VIEWPORT, params);

	GLint width = params[2];
	GLint height = params[3];

	resources::Texture2dInfo tex_info(width, height, resources::pixel_format::rgba8, 4);
	std::vector<uint8_t> data(tex_info.get_data_size());

	std::static_pointer_cast<GlRenderTarget>(this->get_display_target())->bind_read();
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	glReadnPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, tex_info.get_data_size(), data.data());

	resources::Texture2dData img(std::move(tex_info), std::move(data));
	return img.flip_y();
}

void GlRenderer::optimise(const std::shared_ptr<GlRenderPass> &pass) {
	if (!pass->get_is_optimised()) {
		auto renderables = pass->get_renderables();
		std::stable_sort(renderables.begin(), renderables.end(), [](const Renderable& a, const Renderable& b) {
			GLuint shader_a = std::dynamic_pointer_cast<GlShaderProgram>(
				std::dynamic_pointer_cast<GlUniformInput>(a.unif_in)->get_program())->get_handle();
			GLuint shader_b = std::dynamic_pointer_cast<GlShaderProgram>(
				std::dynamic_pointer_cast<GlUniformInput>(b.unif_in)->get_program())->get_handle();
			return shader_a < shader_b;
		});

		pass->set_renderables(renderables);
		pass->set_is_optimised(true);
	}
}

void GlRenderer::render(const std::shared_ptr<RenderPass> &pass) {
	auto gl_target = std::dynamic_pointer_cast<GlRenderTarget>(pass->get_target());
	gl_target->bind_write();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto gl_pass = std::dynamic_pointer_cast<GlRenderPass>(pass);
	GlRenderer::optimise(gl_pass);

	for (auto obj : gl_pass->get_renderables()) {
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

		auto in = std::dynamic_pointer_cast<GlUniformInput>(obj.unif_in);
		auto geom = std::dynamic_pointer_cast<GlGeometry>(obj.geometry);
		std::static_pointer_cast<GlShaderProgram>(in->get_program())->execute_with(in, geom);
	}
}

} // openage::renderer::opengl
