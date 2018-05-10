// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "vertex_array.h"

#include "../../error/error.h"
#include "../../datastructure/constexpr_map.h"


namespace openage {
namespace renderer {
namespace opengl {

/// The type of a single element in a per-vertex attribute.
static constexpr auto gl_types = datastructure::create_const_map<resources::vertex_input_t, GLenum>(
	std::make_pair(resources::vertex_input_t::V2F32, GL_FLOAT),
	std::make_pair(resources::vertex_input_t::V3F32, GL_FLOAT)
);

GlVertexArray::GlVertexArray(std::vector<std::pair<GlBuffer const&, resources::VertexInputInfo const&>> buffers)
	: GlSimpleObject([] (GLuint handle) { glDeleteVertexArrays(1, &handle); } )
{
	GLuint handle;
	glGenVertexArrays(1, &handle);
	this->handle = handle;

	this->bind();

	GLuint attrib = 0;
	for (auto buf_info : buffers) {
		auto const &buf = buf_info.first;
		auto const &info = buf_info.second;

		buf.bind(GL_ARRAY_BUFFER);

		if (info.get_layout() == resources::vertex_layout_t::AOS) {
			size_t offset = 0;

			for (auto in : info.get_inputs()) {
				glEnableVertexAttribArray(attrib);

				glVertexAttribPointer(
					attrib,
					resources::vertex_input_count(in),
					gl_types.get(in),
					GL_FALSE,
					info.vert_size(),
					reinterpret_cast<void*>(offset)
				);

				offset += resources::vertex_input_size(in);
				attrib += 1;
			}
		}
		else if (info.get_layout() == resources::vertex_layout_t::SOA) {
			size_t offset = 0;
			size_t vert_count = buf.get_size() / info.vert_size();

			for (auto in : info.get_inputs()) {
				glEnableVertexAttribArray(attrib);

				glVertexAttribPointer(
					attrib,
					resources::vertex_input_count(in),
					gl_types.get(in),
					GL_FALSE,
					0,
					reinterpret_cast<void*>(offset)
				);

				offset += resources::vertex_input_size(in) * vert_count;
				attrib += 1;
			}

		}
		else {
			throw Error(MSG(err) << "Unknown vertex layout.");
		}
	}
}

GlVertexArray::GlVertexArray(GlBuffer const& buf, resources::VertexInputInfo const& info)
	: GlVertexArray( { { buf, info } } ) {}

GlVertexArray::GlVertexArray()
	: GlSimpleObject([] (GLuint handle) { glDeleteVertexArrays(1, &handle); } )
{
	GLuint handle;
	glGenVertexArrays(1, &handle);
	this->handle = handle;
}

void GlVertexArray::bind() const {
	glBindVertexArray(*this->handle);

	// TODO necessary?
	/*
	// then enable all contained attribute ids
	for (auto &attr_id : this->bound_attributes) {
		glEnableVertexAttribArray(attr_id);
	}
	*/
}

void GlVertexArray::unbind() const {
	glBindVertexArray(0);

}

}}} // openage::renderer::opengl
