// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "mesh_data.h"

#include <cstring>

#include "../../error/error.h"


namespace openage {
namespace renderer {
namespace resources {


VertexInputInfo::VertexInputInfo(std::vector<vertex_input_t> inputs, vertex_layout_t layout)
	: inputs(inputs)
	, layout(layout) {}

/// Returns the size in bytes of a given per-vertex input type.
static size_t vert_in_size(vertex_input_t in) {
	switch(in) {
	case vertex_input_t::V2F32:
		return 8;
	case vertex_input_t::V3F32:
		return 12;
	default:
		throw Error(MSG(err) << "Tried to find size of unknown vertex input type.");
	}
}

size_t VertexInputInfo::size() const {
	size_t size = 0;
	for (auto in : this->inputs) {
		size += vert_in_size(in);
	}
	return size;
}

/// Vertices of a quadrilateral filling the whole screen.
/// Format: (pos, tex_coords) = (x, y, u, v)
static const std::array<float, 16> quad_data = { {
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f
} };

MeshData::MeshData(const util::Path &/*path*/) {
	// asdf
	throw "unimplemented lol";
}

MeshData::MeshData(init_quad_t) {
	auto data_size = quad_data.size() * sizeof(decltype(quad_data)::value_type);
	this->data = std::vector<uint8_t>(data_size);
	std::memcpy(data.data(), reinterpret_cast<const uint8_t*>(quad_data.data()), data_size);

	this->info = { { vertex_input_t::V2F32, vertex_input_t::V2F32 }, vertex_layout_t::AOS };
}

}}}
