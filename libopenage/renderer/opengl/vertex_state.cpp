// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "../../config.h"
#if WITH_OPENGL

#include "vertex_state.h"

#include "../../error/error.h"
#include "../../log/log.h"

namespace openage {
namespace renderer {
namespace opengl {

VertexState::VertexState(/*renderer::Context *ctx*/)
	//:
	//renderer::VertexState{ctx} {
{

	glGenVertexArrays(1, &this->id);
}

VertexState::~VertexState() {
	glDeleteVertexArrays(1, &this->id);
}

void VertexState::attach_buffer(VertexBuffer &buf) {
	// make the vao active, it will store the section assignment
	this->bind();

	// push the buffer to the gpu, if neccessary.
	buf.upload();

	// assign the sections, set the pointers
	for (auto &section : buf.get_sections()) {
		glEnableVertexAttribArray(section.attr_id);

		GLenum type = this->get_attribute_type(section.type);

		glVertexAttribPointer(
			section.attr_id,
			section.dimension,
			type, GL_FALSE, section.stride,
			reinterpret_cast<void *>(section.offset)
		);

		if (this->bound_attributes.count(section.attr_id) > 0) {
			throw Error(MSG(err) << "attribute " << section.attr_id
			            << " already set!");
		}

		// add to the list of active attributes
		this->bound_attributes.insert(section.attr_id);
	}
}

void VertexState::detach_buffer(VertexBuffer &buf) {
	// make the vao active, it will store the section removal
	this->bind();

	// remove sections and attribute ids
	for (auto &section : buf.get_sections()) {
		glDisableVertexAttribArray(section.attr_id);

		// remove from the list of active attributes
		this->bound_attributes.erase(section.attr_id);
	}
}

void VertexState::bind() const {
	// when binding, first enable the vao as state storage
	glBindVertexArray(this->id);

	// then enable all contained attribute ids
	for (auto &attr_id : this->bound_attributes) {
		glEnableVertexAttribArray(attr_id);
	}
}

GLenum VertexState::get_attribute_type(vertex_attribute_type type) {
	switch (type) {
	case vertex_attribute_type::float_32:
		return GL_FLOAT;
	case vertex_attribute_type::integer_32:
		return GL_INT;

	default:
		throw Error{MSG(err) << "unimplemented vertex buffer data type!"};
	}
}


}}} // openage::renderer::opengl

#endif
