// Copyright 2017-2023 the openage authors. See copying.md for legal info.

#include "render_target.h"

#include "renderer/opengl/texture.h"

namespace openage::renderer::opengl {

GlRenderTarget::GlRenderTarget(size_t width, size_t height) :
	type(gl_render_target_t::display),
	size(width, height) {}

GlRenderTarget::GlRenderTarget(const std::shared_ptr<GlContext> &context,
                               const std::vector<std::shared_ptr<GlTexture2d>> &textures) :
	type(gl_render_target_t::textures),
	framebuffer({context, textures}),
	textures(textures) {
	// TODO: Check if the textures are all the same size
	this->size = this->textures.value().at(0)->get_info().get_size();
}

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

void GlRenderTarget::resize(size_t width, size_t height) {
	if (this->type != gl_render_target_t::display) {
		throw Error{ERR << "Texture render target should not be resized. Create a new one instead."};
	}

	this->size = std::make_pair(width, height);
}

void GlRenderTarget::bind_write() const {
	// adjust the viewport to the target size
	// we have to do this because window and texture targets may have
	// different sizes
	glViewport(0, 0, size.first, size.second);

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
