// Copyright 2015-2015 the openage authors. See copying.md for legal info.

/** @file
 * common code for all pipeline programs.
 */

#include "pipeline.h"

#include "vertex_buffer.h"
#include "../util/compiler.h"


namespace openage {
namespace renderer {

PipelineVariable::PipelineVariable(const std::string &name,
                                   Pipeline *pipeline)
	:
	pipeline{pipeline},
	name{name} {

	pipeline->add_var(this);
}

PipelineVariable::~PipelineVariable() {}

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
	program{prg},
	buffer{prg->context} {}

Pipeline::~Pipeline() {}


void Pipeline::add_var(PipelineVariable *var) {
	// just add the variable to the known list
	if (BaseAttribute *attr = dynamic_cast<BaseAttribute *>(var)) {
		this->attributes.push_back(attr);
	} else {
		// non-attribute variable, ignore it.
	}
}


void Pipeline::enqueue_repack() {
	this->attributes_dirty = true;
}

void Pipeline::pack_attribute_buffer() {
	// determine vertex attribute buffer size
	size_t buf_size = 0;

	// number of vertices configured
	size_t vertex_count = 0;

	VertexBuffer *vbuf = &this->buffer;

	// we'll overwrite the whole buffer, so reset the metadata.
	vbuf->reset();

	// gather the expected buffer section and sizes.
	for (auto &var : this->attributes) {
		size_t entry_size = var->entry_size();
		size_t entry_count = var->entry_count();

		// the first attribute determines the expected size.
		if (unlikely(vertex_count == 0)) {
			vertex_count = entry_count;
		}
		else if (unlikely(vertex_count != entry_count)) {
			throw error::Error{MSG(err)
				<< "inconsistent vertex attribute count: expected "
				<< vertex_count << " but " << var->get_name()
				<< " has " << entry_count << " entries."};
		}

		// a new section in the big vbo
		VertexBuffer::vbo_section section{
			var->get_attr_id(),
			entry_size, buf_size};
		vbuf->add_section(section);

		buf_size += entry_size * entry_count;
	}

	// allocate a buffer to hold all the values.
	vbuf->alloc(buf_size);

	// pack the values to the buffer.
	for (size_t i = 0; i < this->attributes.size(); i++) {
		BaseAttribute *var = this->attributes[i];
		VertexBuffer::vbo_section *section = &vbuf->sections[i];

		// store the attribute section to the buffer
		char *pos = vbuf->get() + section->offset;
		var->pack(pos, section->entry_width * vertex_count);
	}
}

void Pipeline::draw() {

	// opengl-pipeline:
	// pack buffer. (vmethod?)
	// upload buffer.
	// check if attributes are active
	// enable/set attributes (vmethod?)
	// draw
	// disable attributes

	// 0. pack buffer
	if (this->attributes_dirty) {
		this->pack_attribute_buffer();
	}

	// TODO: dirtying:
	// upload buffer
	this->program->set_vertex_buffer(this->buffer);

	// next: set attrib ptrs
	// this->program->activate_vertex_buffer(this->buffer);

	/*
	// attribute enabling
	glBindBuffer(GL_ARRAY_BUFFER, vpos_buf);
	glEnableVertexAttribArray(posattr_id);
	glEnableVertexAttribArray(texcoord_id);
	glVertexAttribPointer(posattr_id, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);
	glVertexAttribPointer(texcoord_id, 2, GL_FLOAT, GL_FALSE, 0, (void *)(4 * 4 * 6));

	// draw call
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// attribute disabling
	glDisableVertexAttribArray(posattr_id);
	glDisableVertexAttribArray(texcoord_id);
	*/
}


}} // openage::renderer
