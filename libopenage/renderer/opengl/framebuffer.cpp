// Copyright 2017-2018 the openage authors. See copying.md for legal info.

#include "framebuffer.h"


namespace openage {
namespace renderer {
namespace opengl {

// TODO the validity of this object is contingent
// on its texture existing. use shared_ptr?
GlFramebuffer::GlFramebuffer(std::vector<const GlTexture2d*> textures)
	: GlSimpleObject([] (GLuint handle) { glDeleteFramebuffers(1, &handle); } )
{
	GLuint handle;
	glGenFramebuffers(1, &handle);
	this->handle = handle;

	glBindFramebuffer(GL_FRAMEBUFFER, handle);

	std::vector<GLenum> drawBuffers;

	size_t colorTextureCount = 0;
	for (auto const& texture : textures) {
		// TODO figure out attachment points from pixel formats
		if (texture->get_info().get_format() == resources::pixel_format::depth24) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->get_handle(), 0);
		} else {
			auto attachmentPoint = GL_COLOR_ATTACHMENT0 + colorTextureCount++;
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentPoint, GL_TEXTURE_2D, texture->get_handle(), 0);
			drawBuffers.push_back(attachmentPoint);
		}
	}

	glDrawBuffers(drawBuffers.size(), drawBuffers.data());

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw Error(MSG(err) << "Could not create OpenGL framebuffer.");
	}
}

void GlFramebuffer::bind_read() const {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, *this->handle);
}

void GlFramebuffer::bind_write() const {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, *this->handle);
}

}}} // openage::renderer::opengl
