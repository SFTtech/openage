// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "buffer.h"

#include <epoxy/gl.h>

#include "../../error/error.h"

namespace openage {
namespace renderer {
namespace opengl {

Buffer::Buffer(renderer::Context *ctx, size_t size)
	:
	renderer::Buffer{ctx, size} {

	glGenBuffers(1, &this->id);
}

Buffer::~Buffer() {
	glDeleteBuffers(1, &this->id);
}

void Buffer::upload(bind_target target) const {
	GLenum slot = this->get_target(target);

	glBindBuffer(slot, this->id);
	// submit whole buffer to the GPU
	// TODO: really static draw? probably not.
	glBufferData(slot, this->allocd, this->get(), GL_STATIC_DRAW);
	glBindBuffer(slot, 0);
}

GLenum Buffer::get_target(bind_target target) {
	switch (target) {
	case bind_target::vertex_attributes:
		return GL_ARRAY_BUFFER;
	default:
		throw Error{MSG(err) << "unimplemented buffer binding target!"};
	}
}


}}} // openage::renderer::opengl
