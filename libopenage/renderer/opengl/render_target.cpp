// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#include "render_target.h"


namespace openage::renderer::opengl {

GlRenderTarget::GlRenderTarget() :
	type(gl_render_target_t::display) {}

GlRenderTarget::GlRenderTarget(const std::shared_ptr<GlContext> &context,
                               const std::vector<std::shared_ptr<GlTexture2d>> &textures) :
	type(gl_render_target_t::textures),
	framebuffer({context, textures}),
	textures(textures) {}

std::vector<std::shared_ptr<Texture2d>> GlRenderTarget::get_texture_targets() {
	std::vector<std::shared_ptr<Texture2d>> textures{};
	if (this->type == gl_render_target_t::display) {
		return textures;
	}
	//else upcast pointers
	for (auto tex : this->textures.value()) {
		auto new_ptr = dynamic_pointer_cast<Texture2d>(tex);
		textures.push_back(new_ptr);
	}
	return textures;
}

void GlRenderTarget::bind_write() const {
	if (this->type == gl_render_target_t::textures) {
		this->framebuffer->bind_write();
	}
	else {
		// 0 is the default, window framebuffer
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}
}

void GlRenderTarget::bind_read() const {
	if (this->type == gl_render_target_t::textures) {
		this->framebuffer->bind_read();
	}
	else {
		// 0 is the default, window framebuffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	}
}

} // namespace openage::renderer::opengl
