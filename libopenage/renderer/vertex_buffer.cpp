// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "vertex_buffer.h"

namespace openage {
namespace renderer {

/*VertexBuffer::VertexBuffer(Context *ctx, Buffer::usage usage)
	:
	usage{usage} {

	this->buffer = ctx->create_buffer();
}
*/
VertexBuffer::VertexBuffer(Buffer *bufferPtr, Buffer::usage usage)
	:
	usage{usage} {

	this->buffer = std::unique_ptr<Buffer>(bufferPtr);
}

VertexBuffer::VertexBuffer(VertexBuffer &&other)
	:
	sections{std::move(other.sections)},
	buffer{std::move(other.buffer)},
	usage{other.usage} {}

const VertexBuffer &VertexBuffer::operator =(VertexBuffer &&other) {
	sections = std::move(other.sections);
	buffer = std::move(other.buffer);
	usage = other.usage;
	return *this;
}

void VertexBuffer::upload() {
	this->buffer->upload(Buffer::bind_target::vertex_attributes, this->usage);
}

void VertexBuffer::alloc(size_t size) {
	this->buffer->create(size);
}

void VertexBuffer::reset() {
	this->sections.clear();
}

void VertexBuffer::add_section(const vbo_section &section) {
	this->sections.push_back(section);
}

char *VertexBuffer::get(bool will_modify) {
	return this->buffer->get(will_modify);
}

Buffer *VertexBuffer::get_buffer() {
	return this->buffer.get();
}

const std::vector<VertexBuffer::vbo_section> &VertexBuffer::get_sections() const {
	return this->sections;
}


}} // openage::renderer
