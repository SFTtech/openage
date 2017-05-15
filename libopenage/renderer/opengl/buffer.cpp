// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "buffer.h"

#include "../../error/error.h"


namespace openage {
namespace renderer {
namespace opengl {

GlBuffer::GlBuffer(size_t size, GLenum usage)
	: size(size)
	, usage(usage) {
	GLuint id;
	glGenBuffers(1, &id);
	this->id = id;

	this->bind(GL_ARRAY_BUFFER);
	glBufferData(GL_ARRAY_BUFFER, size, 0, usage);
}

GlBuffer::GlBuffer(const uint8_t *data, size_t size, GLenum usage)
	: size(size)
	, usage(usage) {
	GLuint id;
	glGenBuffers(1, &id);
	this->id = id;

	this->bind(GL_ARRAY_BUFFER);
	glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}

GlBuffer::GlBuffer(GlBuffer &&other)
	: id(other.id)
	, size(other.size)
	, usage(other.usage) {
	other.id = std::experimental::optional<GLuint>();
}

GlBuffer &GlBuffer::operator =(GlBuffer &&other) {
	this->id = other.id;
	this->size = other.size;
	this->usage = other.usage;
	other.id = std::experimental::optional<GLuint>();

	return *this;
}

GlBuffer::~GlBuffer() {
	if (this->id) {
		glDeleteBuffers(1, &this->id.value());
	}
}

size_t GlBuffer::get_size() const {
	return this->size;
}

void GlBuffer::upload_data(const uint8_t *data, size_t offset, size_t size) {
	if (unlikely(offset + size > this->size)) {
		throw Error(MSG(err) << "Tried to upload more data to OpenGL buffer than can fit.");
	}

	this->bind(GL_ARRAY_BUFFER);
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

void GlBuffer::bind(GLenum target) const {
	glBindBuffer(target, *this->id);
}

}}} // openage::renderer::opengl
