// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "renderer.h"

#include "../../log/log.h"
#include "../../error/error.h"
#include "texture.h"
#include "shader_program.h"
#include "uniform_input.h"
#include "geometry.h"
//#include "sprite.h"

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

std::shared_ptr<ShaderProgram> GlRenderer::add_shader(std::vector<resources::ShaderSource> const& srcs) {
	log::log(INFO << "max texture slots "<<this->gl_context->get_capabilities().max_texture_slots);
	return std::make_shared<GlShaderProgram>(srcs, this->gl_context->get_capabilities());
}

std::shared_ptr<Geometry> GlRenderer::add_mesh_geometry(resources::MeshData const& mesh) {
	return std::make_shared<GlGeometry>(mesh);
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

/*Renderable make_sprite(util::Path path,char tex_path[],int subtex,bool use_metafile,std::vector<resources::ShaderSource> const& srcs,float aspect,float scale,Eigen::Vector3f center_coord){
		Sprite sprite;
		auto obj = sprite.create(path, tex_path,subtex,use_metafile, srcs,aspect,scale,center_coord);
		return obj;
		//sprite.hello();
}*/


RenderTarget const* GlRenderer::get_display_target() {
	return &this->display;
}

resources::TextureData GlRenderer::display_into_data() {
	GLint params[4];
	glGetIntegerv(GL_VIEWPORT, params);

	GLint width = params[2];
	GLint height = params[3];

	resources::TextureInfo tex_info(width, height, resources::pixel_format::rgba8, 4);
	std::vector<uint8_t> data(tex_info.get_data_size());

	static_cast<GlRenderTarget const*>(this->get_display_target())->bind_read();
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	glReadnPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, tex_info.get_data_size(), data.data());

	resources::TextureData img(std::move(tex_info), std::move(data));
	return img.flip_y();
}

void GlRenderer::render(RenderPass const& pass) {
	log::log(INFO << "Render 0");
	auto gl_target = dynamic_cast<const GlRenderTarget*>(pass.target);
	gl_target->bind_write();
	log::log(INFO << "Render 1");
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto obj : pass.renderables) {
		log::log(INFO << "Render 3");
		if (obj.alpha_blending) {
			log::log(INFO << "Render 4");
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
		log::log(INFO << "Render 5");


		auto in = dynamic_cast<GlUniformInput const*>(obj.unif_in);
		log::log(INFO << "Render 5.5");
		auto geom = dynamic_cast<GlGeometry const*>(obj.geometry);

		log::log(INFO << "Render 6");
		in->program->execute_with(in, geom);
		log::log(INFO << "Render 7");
	}
}

void GlRenderer::render_test(RenderPass_test const& pass) {
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


		auto in = dynamic_cast<GlUniformInput const*>((obj.unif_in).get());
		auto geom = dynamic_cast<GlGeometry const*>((obj.geometry).get());

		in->program->execute_with(in, geom);
	}
}


}}} // openage::renderer::opengl
