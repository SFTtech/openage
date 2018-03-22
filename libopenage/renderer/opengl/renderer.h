// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include <SDL2/SDL.h>

#include "context.h"
#include "../renderer.h"
#include "shader_program.h"
#include "render_target.h"


namespace openage {
namespace renderer {
namespace opengl {
class Sprite;
/// The OpenGL specialization of the rendering interface.
class GlRenderer final : public Renderer {
public:
	GlRenderer(GlContext*);

	std::unique_ptr<Texture> add_texture(resources::TextureData const&) override;
	//std::unique_ptr<Renderable> make_sprite(util::Path& path,char tex_path[],int subtex,bool use_metafile,std::vector<resources::ShaderSource> const& srcs,float aspect,float scale,Eigen::Vector3f center_coord);
	std::unique_ptr<Texture> add_texture(resources::TextureInfo const&) override;

	std::shared_ptr<ShaderProgram> add_shader(std::vector<resources::ShaderSource> const&) override;

	std::shared_ptr<Geometry> add_mesh_geometry(resources::MeshData const&) override;
	std::unique_ptr<Geometry> add_bufferless_quad() override;

	std::unique_ptr<RenderTarget> create_texture_target(std::vector<Texture*>) override;
	RenderTarget const* get_display_target() override;

	resources::TextureData display_into_data() override;

	void render(RenderPass const&) override;
	void render_test(RenderPass_test const&) override;

private:
	/// The GL context.
	GlContext *gl_context;

	GlRenderTarget display;
};

}}} // openage::renderer::opengl
