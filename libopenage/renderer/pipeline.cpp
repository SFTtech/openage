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
void Uniform<Texture *>::upload_value() {
	this->pipeline->get_program()->set_uniform_2dtexture(this->get_name_cstr(), this->value);
}


template<>
void Uniform<int>::upload_value() {
	this->pipeline->get_program()->set_uniform_1i(this->get_name_cstr(), this->value);
}


template<>
void Uniform<bool>::upload_value() {
	unsigned val = this->value ? 1 : 0;
	this->pipeline->get_program()->set_uniform_1ui(this->get_name_cstr(), val);
}


Pipeline::Pipeline(Program *prg)
	:
	program{prg} {}


Pipeline::Pipeline(Pipeline &&other)
	:
	program{other.program} {

	this->update_proplists(other);
}

Pipeline::Pipeline(const Pipeline &other)
	:
	program{other.program} {

	this->update_proplists(other);
}

Pipeline &Pipeline::operator =(Pipeline &&other) {
	this->program = other.program;
	this->update_proplists(other);
	return *this;
}

Pipeline &Pipeline::operator =(const Pipeline &other) {
	this->program = other.program;
	this->update_proplists(other);
	return *this;
}

Program *Pipeline::get_program() {
	return this->program;
}

void Pipeline::add_var(PipelineVariable *var) {
	if (BaseAttribute *attr = dynamic_cast<BaseAttribute *>(var)) {
		this->attributes.push_back(attr);
	}
	else if (BaseUniform *unif = dynamic_cast<BaseUniform *>(var)) {
		this->uniforms.push_back(unif);
	}
	else {
		// unknown variable, ignore it.
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
	// TODO: use buffersubdata to only transfer the modified
	//       parts of the buffer.

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


void Pipeline::upload_uniforms() {
	for (auto &uniform : this->uniforms) {
		uniform->upload();
	}
}


void Pipeline::update_proplists(const Pipeline &source) {
	this->update_proplistptr<>(&this->attributes, &source, source.attributes);
	this->update_proplistptr<>(&this->uniforms, &source, source.uniforms);
}


template <typename T>
void Pipeline::update_proplistptr(std::vector<T *> *proplist_dest,
                                  const Pipeline *source,
                                  const std::vector<T *> &proplist_src) {
	// the proplist stores pointers to class members.
	// now we're a new class: the pointers need to be updated
	// to point to the members of the new class.
	// The offsets are the same, the base address is different (a new 'this').
	proplist_dest->clear();

	// beware: ultra dirty hackery.
	// the new variable offset is calculated from the old one.
	// all because c++ has no reflection to iterate over member variables.
	size_t new_base = reinterpret_cast<size_t>(this);
	for (auto &entry : proplist_src) {
		size_t distance = reinterpret_cast<size_t>(entry) - reinterpret_cast<size_t>(source);
		proplist_dest->push_back(reinterpret_cast<T *>(new_base + distance));
	}
}

}} // openage::renderer
