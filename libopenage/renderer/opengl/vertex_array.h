// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#pragma once

#include "../resources/mesh_data.h"
#include "buffer.h"
#include "simple_object.h"


namespace openage {
namespace renderer {
namespace opengl {

/// An OpenGL Vertex Array Object. It represents a mapping between a buffer
/// that contains vertex data and a way for the context to interpret it.
class GlVertexArray final : public GlSimpleObject {
public:
	/// Initializes a vertex array from the given mesh data.
	/// If no shader input mapping is present, the inputs are parsed as follows - each subsequent
	/// (GlBuffer, VertexInputInfo) pair is added to the VAO in the same order as they appear
	/// in the vector. Each buffer is bound according to its input info and each subsequent vertex
	/// attribute is attached to an id in ascending order.
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
	///
	/// A shader input mapping is only allowed when there is a single element in `buffers`. In such a case,
	/// the vertex inputs are paired with VAO attributes according to the mapping instead of in ascending order.
	GlVertexArray(std::vector<std::pair<GlBuffer const&, resources::VertexInputInfo const&>> buffers);

	/// Executes the buffer list constructor with one element.
	GlVertexArray(GlBuffer const&, resources::VertexInputInfo const&);

	/// The default constructor initializes an empty VAO with no attributes.
	/// This is useful for bufferless drawing.
	GlVertexArray();

	/// Make this vertex array object the current one.
	void bind() const;
};

}}} // openage::renderer::opengl
