// Copyright 2015-2015 the openage authors. See copying.md for legal info.

/** @file
 * common code for all pipeline programs.
 */

#include "pipeline.h"

#include "context.h"
#include "vertex_buffer.h"
#include "vertex_state.h"
#include "../log/log.h"
#include "../util/compiler.h"

#include <epoxy/gl.h>
#include <typeinfo>

namespace openage {
namespace renderer {

PipelineVariable::PipelineVariable(const std::string &name,
                                   Pipeline *pipeline)
	:
	pipeline{pipeline},
	name{name} {}

const std::string &PipelineVariable::get_name() {
	return this->name;
}

const char *PipelineVariable::get_name_cstr() {
	return this->name.c_str();
}


template<>
void Uniform<Texture>::set(const Texture &value) {
	this->pipeline->program->set_uniform_2dtexture(this->get_name_cstr(), value);
}

template<>
void Uniform<int>::set(const int &value) {
	this->pipeline->program->set_uniform_1i(this->get_name_cstr(), value);
}


Pipeline::Pipeline(Program *prg)
	:
	program{prg} {}


void Pipeline::add_var(PipelineVariable *var) {
	if (BaseAttribute *attr = dynamic_cast<BaseAttribute *>(var)) {
		// just add the variable to the known list
		this->attributes.push_back(attr);
	} else {
		// non-attribute variable, ignore it.
	}
}


VertexBuffer Pipeline::create_attribute_buffer() {
	// create a fresh buffer
	VertexBuffer vbuf{this->program->context};

	// fill the buffer with the current vertex data.
	this->update_buffer(&vbuf);

	return vbuf;
}


void Pipeline::update_buffer(VertexBuffer *vbuf) {
	// determine vertex attribute buffer size
	size_t buf_size = 0;

	// number of vertices configured
	size_t vertex_count = 0;

	// we'll overwrite the whole buffer, so reset the metadata.
	vbuf->reset();

	// gather the expected buffer section and sizes.
	for (auto &var : this->attributes) {
		size_t entry_count = var->entry_count();

		// the first attribute determines the expected size.
		// all other attribute-definitions will have to provide the same
		// number of entries (so that all vertices can get the attributes).
		if (unlikely(vertex_count == 0)) {
			vertex_count = entry_count;
		}
		else if (unlikely(vertex_count != entry_count)) {
			throw error::Error{MSG(err)
				<< "inconsistent vertex attribute count: expected "
				<< vertex_count << " but " << var->get_name()
				<< " has " << entry_count << " entries."};
		}

		// a new section in the big vertex buffer
		VertexBuffer::vbo_section section{
			var->get_attr_id(),
			var->type(),
			var->dimension(),
			buf_size   // current buffer size, increased for each section.
		};
		vbuf->add_section(section);

		buf_size += var->entry_size() * entry_count;
	}

	// allocate a buffer to hold all the values.
	// the next loop will fill into that memory.
	vbuf->alloc(buf_size);

	auto sections = vbuf->get_sections();

	// pack the values to the buffer.
	for (size_t i = 0; i < this->attributes.size(); i++) {
		BaseAttribute *var = this->attributes[i];
		VertexBuffer::vbo_section *section = &sections[i];

		// raw pointer to to-be-submitted data buffer.
		char *pos = vbuf->get(true);
		pos += section->offset;

		// store the attribute section to the buffer
		var->pack(pos, var->entry_size() * vertex_count);
	}
}

}} // openage::renderer
