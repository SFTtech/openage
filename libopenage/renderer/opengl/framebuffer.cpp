// Copyright 2017-2025 the openage authors. See copying.md for legal info.

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

	std::vector<GLenum> draw_buffers;

	if (textures.empty()) {
		throw Error{ERR << "At least 1 texture must be assigned to texture framebuffer."};
	}

	size_t color_texture_count = 0;
	size_t depth_texture_count = 0;
	for (auto const &texture : textures) {
		auto fmt = texture->get_info().get_format();
		switch (fmt) {
		case resources::pixel_format::depth24:
			depth_texture_count += 1;
			if (depth_texture_count > 1) {
				log::log(WARN << "Framebuffer already has one depth texture attached. "
				              << "Assignment of additional depth texture ignored.");
				break;
			}
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->get_handle(), 0);
			break;
		case resources::pixel_format::r16ui:
		case resources::pixel_format::r32ui:
		case resources::pixel_format::rgba8:
		case resources::pixel_format::rgb8:
		case resources::pixel_format::bgr8:
		case resources::pixel_format::rgba8ui: {
			auto attachment_point = GL_COLOR_ATTACHMENT0 + color_texture_count++;
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_point, GL_TEXTURE_2D, texture->get_handle(), 0);
			draw_buffers.push_back(attachment_point);
		} break;
		default:
			throw Error{ERR << "Unsupported pixel format for framebuffer texture."};
		}
	}

	glDrawBuffers(draw_buffers.size(), draw_buffers.data());

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
