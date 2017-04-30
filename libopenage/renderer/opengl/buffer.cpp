// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "../../config.h"
#if WITH_OPENGL

#include "buffer.h"

#include <epoxy/gl.h>

#include "../../error/error.h"

namespace openage {
namespace renderer {
namespace opengl {

Buffer::Buffer(/*renderer::Context *ctx,*/ size_t size)
	:
	renderer::Buffer{/*ctx,*/ size} {

	glGenBuffers(1, &this->id);
}

Buffer::~Buffer() {
	glDeleteBuffers(1, &this->id);
}

void Buffer::upload(bind_target target, usage usage) {
	GLenum gl_usage = this->get_usage(usage);
	GLenum gl_slot = this->get_target(target);

	if (not this->on_gpu) {
		this->bind(target);
		glBufferData(gl_slot, this->allocd, this->get(), gl_usage);
		this->on_gpu = true;
	}
}

void Buffer::bind(bind_target target) const {
	GLenum gl_slot = this->get_target(target);
	// TODO: ask the context if already bound.
	glBindBuffer(gl_slot, this->id);
}


GLenum Buffer::get_target(bind_target target) {
	switch (target) {
	case bind_target::vertex_attributes:
		return GL_ARRAY_BUFFER;
	case bind_target::element_indices:
		return GL_ELEMENT_ARRAY_BUFFER;
	case bind_target::query:
		return GL_QUERY_BUFFER;
	default:
		throw Error{MSG(err) << "unimplemented buffer binding target!"};
	}
}

GLenum Buffer::get_usage(usage u) {
	switch (u) {
	case usage::static_draw:
		return GL_STATIC_DRAW;
	case usage::static_read:
		return GL_STATIC_READ;
	case usage::stream_draw:
		return GL_STREAM_DRAW;
	case usage::stream_read:
		return GL_STREAM_READ;
	case usage::stream_copy:
		return GL_STREAM_COPY;
	case usage::static_copy:
		return GL_STATIC_COPY;
	case usage::dynamic_draw:
		return GL_DYNAMIC_DRAW;
	case usage::dynamic_read:
		return GL_DYNAMIC_READ;
	case usage::dynamic_copy:
		return GL_DYNAMIC_COPY;

	default:
		throw Error{MSG(err) << "unimplemented buffer usage prediction!"};
	}
}

}}} // openage::renderer::opengl

#endif
