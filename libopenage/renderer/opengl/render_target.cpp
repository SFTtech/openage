// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "render_target.h"


namespace openage {
namespace renderer {
namespace opengl {

GlRenderTarget::GlRenderTarget()
	: type(gl_render_target_t::display) {}

GlRenderTarget::GlRenderTarget(std::vector<const GlTexture2d*> textures)
	: type(gl_render_target_t::textures)
	, framebuffer(textures) {}

void GlRenderTarget::bind_write() const {
	if (this->type == gl_render_target_t::textures) {
		this->framebuffer->bind_write();
	}
	else {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}
}

void GlRenderTarget::bind_read() const {
	if (this->type == gl_render_target_t::textures) {
		this->framebuffer->bind_read();
	}
	else {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	}
}

}}} // openage::renderer::opengl
