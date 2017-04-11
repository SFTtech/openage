// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include <SDL2/SDL.h>

#include "context.h"
#include "../renderer.h"
#include "shader_program.h"


namespace openage {
namespace renderer {
namespace opengl {

class GlRenderer : public Renderer {
public:
	GlRenderer(GlContext*);

	std::unique_ptr<Texture> add_texture(resources::TextureData const&) override;

	std::unique_ptr<ShaderProgram> add_shader(std::vector<resources::ShaderSource> const&) override;

	std::unique_ptr<RenderTarget> create_texture_target(Texture const*) override;
	RenderTarget const* get_framebuffer_target() override;

	void render(RenderPass const&) override;

private:
	/* The GL context. */
	GlContext *gl_context;

	std::vector<GlShaderProgram> shaders;

	std::unique_ptr<RenderTarget> framebuffer;
};

}}} // openage::renderer::opengl
