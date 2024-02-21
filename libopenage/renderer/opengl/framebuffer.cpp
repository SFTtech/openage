// Copyright 2017-2024 the openage authors. See copying.md for legal info.

#include "framebuffer.h"

#include "log/log.h"

#include "renderer/opengl/context.h"
#include "renderer/opengl/texture.h"


namespace openage::renderer::opengl {

GlFramebuffer::GlFramebuffer(const std::shared_ptr<GlContext> &context) :
	GlSimpleObject(context,
                   [](GLuint /*handle*/) {}),
	type{gl_framebuffer_t::display} {
	log::log(MSG(dbg) << "Created OpenGL framebuffer with display target");
}

// TODO the validity of this object is contingent
// on its texture existing. use shared_ptr?
GlFramebuffer::GlFramebuffer(const std::shared_ptr<GlContext> &context,
                             std::vector<std::shared_ptr<GlTexture2d>> const &textures) :
	GlSimpleObject(context,
                   [](GLuint handle) { glDeleteFramebuffers(1, &handle); }),
	type{gl_framebuffer_t::textures} {
	GLuint handle;
	glGenFramebuffers(1, &handle);
	this->handle = handle;

	glBindFramebuffer(GL_FRAMEBUFFER, handle);

	std::vector<GLenum> drawBuffers;

	if (textures.empty()) {
		throw Error{ERR << "At least 1 texture must be assigned to texture framebuffer."};
	}

	size_t colorTextureCount = 0;
	for (auto const &texture : textures) {
		// TODO figure out attachment points from pixel formats
		if (texture->get_info().get_format() == resources::pixel_format::depth24) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->get_handle(), 0);
		}
		else {
			auto attachmentPoint = GL_COLOR_ATTACHMENT0 + colorTextureCount++;
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentPoint, GL_TEXTURE_2D, texture->get_handle(), 0);
			drawBuffers.push_back(attachmentPoint);
		}
	}

	glDrawBuffers(drawBuffers.size(), drawBuffers.data());

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw Error(MSG(err) << "Could not create OpenGL framebuffer.");
	}

	log::log(MSG(dbg) << "Created OpenGL framebuffer with texture targets");
}

gl_framebuffer_t GlFramebuffer::get_type() const {
	return this->type;
}

void GlFramebuffer::bind_read() const {
	if (this->type == gl_framebuffer_t::textures) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, *this->handle);
	}
	else {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, this->context->get_default_framebuffer_id());
	}
}

void GlFramebuffer::bind_write() const {
	if (this->type == gl_framebuffer_t::textures) {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, *this->handle);
	}
	else {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->context->get_default_framebuffer_id());
	}
}

} // namespace openage::renderer::opengl
