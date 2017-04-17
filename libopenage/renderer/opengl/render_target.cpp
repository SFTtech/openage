// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "render_target.h"


namespace openage {
namespace renderer {
namespace opengl {

GlRenderTarget::GlRenderTarget()
	: type(gl_render_target_t::display)
	, handle(0) {}

// TODO the validity of this object is contingent
// on its texture existing. use shared_ptr?
GlRenderTarget::GlRenderTarget(std::vector<const GlTexture*> textures) {
	glGenFramebuffers(1, &*this->handle);
	glBindFramebuffer(GL_FRAMEBUFFER, *this->handle);
		
	std::vector<GLenum> drawBuffers;

	for (size_t i = 0; i < textures.size(); i++) {
		// TODO figure out attachment points from pixel formats
		auto attachmentPoint = GL_COLOR_ATTACHMENT0 + i;
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentPoint, GL_TEXTURE_2D, textures[i]->get_handle(), 0);
		drawBuffers.push_back(attachmentPoint);
	}

	glDrawBuffers(drawBuffers.size(), drawBuffers.data());

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw Error(MSG(err) << "Could not create OpenGL framebuffer.");
	}
}

GlRenderTarget::GlRenderTarget(GlRenderTarget &&other)
	: type(other.type)
	, handle(other.handle) {
	other.handle = std::experimental::optional<GLuint>();
}

GlRenderTarget &GlRenderTarget::operator =(GlRenderTarget&& other) {
	if (this->type == gl_render_target_t::textures && this->handle) {
		glDeleteFramebuffers(1, &*this->handle);
	}

	this->type = other.type;
	this->handle = other.handle;
	other.handle = std::experimental::optional<GLuint>();

	return *this;
}

GlRenderTarget::~GlRenderTarget() {
	if (this->type == gl_render_target_t::textures && this->handle) {
		glDeleteFramebuffers(1, &*this->handle);
	}
}

void GlRenderTarget::bind_read() const {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, *this->handle);
}

void GlRenderTarget::bind_write() const {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, *this->handle);
}

}}}
