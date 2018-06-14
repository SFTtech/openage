// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "buffer.h"

#include "../../error/error.h"


namespace openage {
namespace renderer {
namespace opengl {

GlBuffer::GlBuffer(size_t size, GLenum usage)
	: GlSimpleObject([] (GLuint handle) { glDeleteBuffers(1, &handle); } )
	, size(size)
	, usage(usage) {
	GLuint handle;
	glGenBuffers(1, &handle);
	this->handle = handle;

	this->bind(GL_COPY_WRITE_BUFFER);
	glBufferData(GL_COPY_WRITE_BUFFER, size, 0, usage);
}

GlBuffer::GlBuffer(const uint8_t *data, size_t size, GLenum usage)
	: GlSimpleObject([] (GLuint handle) { glDeleteBuffers(1, &handle); } )
	, size(size)
	, usage(usage) {
	GLuint handle;
	glGenBuffers(1, &handle);
	this->handle = handle;

	this->bind(GL_COPY_WRITE_BUFFER);
	glBufferData(GL_COPY_WRITE_BUFFER, size, data, usage);
}

size_t GlBuffer::get_size() const {
	return this->size;
}

void GlBuffer::upload_data(const uint8_t *data, size_t offset, size_t size) {
	if (unlikely(offset + size > this->size)) {
		throw Error(MSG(err) << "Tried to upload more data to OpenGL buffer than can fit.");
	}

	this->bind(GL_COPY_WRITE_BUFFER);
	glBufferSubData(GL_COPY_WRITE_BUFFER, offset, size, data);
}

void GlBuffer::bind(GLenum target) const {
	glBindBuffer(target, *this->handle);
}

}}} // openage::renderer::opengl
