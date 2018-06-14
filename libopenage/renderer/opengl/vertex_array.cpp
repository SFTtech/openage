// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "vertex_array.h"

#include <map>

#include "../../error/error.h"
#include "../../datastructure/constexpr_map.h"

#include "lookup.h"


namespace openage {
namespace renderer {
namespace opengl {

GlVertexArray::GlVertexArray(std::vector<std::pair<GlBuffer const&, resources::VertexInputInfo const&>> buffers)
	: GlSimpleObject([] (GLuint handle) { glDeleteVertexArrays(1, &handle); } )
{
	GLuint handle;
	glGenVertexArrays(1, &handle);
	this->handle = handle;

	this->bind();

	if (buffers.size() == 1 and buffers[0].second.get_shader_input_map()) { // Map inputs according to specified pairing
		auto const& info = buffers[0].second;
		auto const& in_map = *info.get_shader_input_map();
		auto const& in = info.get_inputs();

		auto& buf = buffers[0].first;
		buf.bind(GL_ARRAY_BUFFER);

		std::map<size_t, size_t> in_map_sorted(in_map.begin(), in_map.end());

		size_t offset = 0;
		size_t next_idx = 0;

		if (info.get_layout() == resources::vertex_layout_t::AOS) {
			for (auto mapping : in_map_sorted) {
				if (mapping.first != next_idx) {
					// Some attributes were skipped, so add them to the offset without enabling in VAO
					for (size_t i = next_idx; i < mapping.first; i++) {
						offset += resources::vertex_input_size(in[i]);
					}
				}

				glVertexAttribPointer(
					mapping.second,
					resources::vertex_input_count(in[mapping.first]),
					GL_VERT_IN_ELEM_TYPE.get(in[mapping.first]),
					GL_FALSE,
					info.vert_size(),
					reinterpret_cast<void*>(offset)
				);

				offset += resources::vertex_input_size(in[mapping.first]);
				next_idx = mapping.first + 1;
			}
		} else if (info.get_layout() == resources::vertex_layout_t::SOA) {
			size_t vert_count = buf.get_size() / info.vert_size();

			for (auto mapping : in_map_sorted) {
				if (mapping.first != next_idx) {
					// Some attributes were skipped, so add them to the offset without enabling in VAO
					for (size_t i = next_idx; i < mapping.first; i++) {
						offset += resources::vertex_input_size(in[i]) * vert_count;
					}
				}

				glVertexAttribPointer(
					mapping.second,
					resources::vertex_input_count(in[mapping.first]),
					GL_VERT_IN_ELEM_TYPE.get(in[mapping.first]),
					GL_FALSE,
					0,
					reinterpret_cast<void*>(offset)
				);

				offset += resources::vertex_input_size(in[mapping.first]) * vert_count;
				next_idx = mapping.first + 1;
			}
		} else {
			throw Error(MSG(err) << "Unknown vertex format.");
		}
	} else { // Map inputs in ascending order
		GLuint attrib = 0;

		for (auto buf_info : buffers) {
			auto const &buf = buf_info.first;
			auto const &info = buf_info.second;

			if (unlikely(info.get_shader_input_map())) {
				throw Error(MSG(err) << "Shader input mapping is unsupported when constructing a VAO from multiple buffers.");
			}

			buf.bind(GL_ARRAY_BUFFER);

			if (info.get_layout() == resources::vertex_layout_t::AOS) {
				size_t offset = 0;

				for (auto in : info.get_inputs()) {
					glEnableVertexAttribArray(attrib);

					glVertexAttribPointer(
						attrib,
						resources::vertex_input_count(in),
						GL_VERT_IN_ELEM_TYPE.get(in),
						GL_FALSE,
						info.vert_size(),
						reinterpret_cast<void*>(offset)
					);

					offset += resources::vertex_input_size(in);
					attrib += 1;
				}
			} else if (info.get_layout() == resources::vertex_layout_t::SOA) {
				size_t offset = 0;
				size_t vert_count = buf.get_size() / info.vert_size();

				for (auto in : info.get_inputs()) {
					glEnableVertexAttribArray(attrib);

					glVertexAttribPointer(
						attrib,
						resources::vertex_input_count(in),
						GL_VERT_IN_ELEM_TYPE.get(in),
						GL_FALSE,
						0,
						reinterpret_cast<void*>(offset)
					);

					offset += resources::vertex_input_size(in) * vert_count;
					attrib += 1;
				}
			} else {
				throw Error(MSG(err) << "Unknown vertex layout.");
			}
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

}}} // openage::renderer::opengl
