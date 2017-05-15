// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <epoxy/gl.h>
#include <experimental/optional>

#include "../resources/mesh_data.h"
#include "buffer.h"


namespace openage {
namespace renderer {
namespace opengl {

/// An OpenGL Vertex Array Object. It represents a mapping between a buffer
/// that contains vertex data and a way for the context to interpret it.
class GlVertexArray  {
public:
	/// Initializes a vertex array from the given mesh data.
	/// The inputs are parsed as follows - each subsequent (GlBuffer, VertexInputInfo) pair is added
	/// to the VAO in the same order as they appear in the vector. Each buffer is bound according to its
	/// input info and each subsequent vertex attribute is attached to an id in ascending order.
	/// For example, the inputs:
	/// in vec3 pos;
	/// in vec2 uv;
	/// in mat3 rot;
	/// could be bound to two buffers like so:
	/// GlBuffer buf1(data1, size1);
	/// GlBuffer buf2(data2, size2);
	/// GlVertexArray( {
	/// 	std::make_pair(buf1, { { vertex_input_t::V3F32, vertex_input_t::V2F32 }, vertex_layout_t::SOA },
	/// 	std::make_pair(buf2, { { vertex_input_t::M3F32 }, vertex_input_t::AOS /*or ::SOA, doesn't matter*/ },
	/// } );
	/// Not that anyone would ever want to bind more than one buffer to a single mesh..
	GlVertexArray(std::vector<std::pair<GlBuffer const&, resources::VertexInputInfo const&>> buffers);

	/// Executes the buffer list constructor with one element.
	GlVertexArray(GlBuffer const&, resources::VertexInputInfo const&);

	/// The default constructor initializes an empty VAO with no attributes.
	/// This is useful for bufferless drawing.
	GlVertexArray();

	GlVertexArray(GlVertexArray&&);
	GlVertexArray &operator =(GlVertexArray&&);

	GlVertexArray(GlVertexArray const&) = delete;
	GlVertexArray &operator =(GlVertexArray const&) = delete;

	~GlVertexArray();

	/// Make this vertex array object the current one.
	void bind() const;

protected:
	/// OpenGL handle to this object.
	std::experimental::optional<GLuint> id;
};

}}} // openage::renderer::opengl
