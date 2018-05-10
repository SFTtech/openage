// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "indexbuffer.h"

#include "../../error/error.h"


namespace openage {
namespace renderer {
namespace opengl {

GlIndexBuffer::GlIndexBuffer(size_t size, GLenum usage)
	: GlSimpleObject([] (GLuint handle) { glDeleteBuffers(1, &handle); } )
	, size(size)
	, usage(usage) {
	GLuint handle;
	glGenBuffers(1, &handle);
	this->handle = handle;

	this->bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, 0, usage);
}

GlIndexBuffer::GlIndexBuffer(const GLuint *data, size_t size, GLenum usage)
	: GlSimpleObject([] (GLuint handle) { glDeleteBuffers(1, &handle); } )
	, size(size)
	, usage(usage) {
	GLuint handle;
	glGenBuffers(1, &handle);
	this->handle = handle;

	this->bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
}

size_t GlIndexBuffer::get_size() const {
	return this->size;
}

void GlIndexBuffer::upload_data(const GLuint *data, size_t offset, size_t size) {
	if (unlikely(offset + size > this->size)) {
		throw Error(MSG(err) << "Tried to upload more data to OpenGL buffer than can fit.");
	}

	this->bind();
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
}

void GlIndexBuffer::bind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *this->handle);
}

void GlIndexBuffer::unbind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}}} // openage::renderer::opengl
