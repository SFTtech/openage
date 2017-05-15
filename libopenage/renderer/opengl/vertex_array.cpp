// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "vertex_array.h"

#include "../../error/error.h"
#include "../../datastructure/constexpr_map.h"


namespace openage {
namespace renderer {
namespace opengl {

static constexpr auto gl_types = datastructure::create_const_map<resources::vertex_input_t, GLenum>(
	std::make_pair(resources::vertex_input_t::V2F32, GL_FLOAT),
	std::make_pair(resources::vertex_input_t::V3F32, GL_FLOAT)
);

GlVertexArray::GlVertexArray(std::vector<std::pair<GlBuffer const&, resources::VertexInputInfo const&>> buffers) {
	GLuint id;
	glGenVertexArrays(1, &id);
	this->id = id;

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

GlVertexArray::GlVertexArray() {
	GLuint id;
	glGenVertexArrays(1, &id);
	this->id = id;
}

GlVertexArray::GlVertexArray(GlVertexArray &&other)
	: id(other.id) {
	other.id = {};
}

GlVertexArray &GlVertexArray::operator =(GlVertexArray &&other) {
	this->id = other.id;
	other.id = {};

	return *this;
}

GlVertexArray::~GlVertexArray() {
	if (this->id) {
		glDeleteVertexArrays(1, &this->id.value());
	}
}

void GlVertexArray::bind() const {
	glBindVertexArray(*this->id);

	// TODO necessary?
	/*
	// then enable all contained attribute ids
	for (auto &attr_id : this->bound_attributes) {
		glEnableVertexAttribArray(attr_id);
	}
	*/
}

}}} // openage::renderer::opengl
