// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "geometry.h"
#include "vertex_state.h"
#include "vertex_buffer.h"

namespace openage {
namespace renderer {

const std::array<Geometry::Vertex, 4> Geometry::quad_geometry{{
	{ Eigen::Vector2f(-1.0f, 1.0f), Eigen::Vector2f(0.0f, 1.0f) },
	{ Eigen::Vector2f(-1.0f, -1.0f), Eigen::Vector2f(0.0f, 0.0f) },
	{ Eigen::Vector2f(1.0f, 1.0f), Eigen::Vector2f(1.0f, 1.0f) },
	{ Eigen::Vector2f(1.0f, -1.0f), Eigen::Vector2f(1.0f, 0.0f) }
}};

Geometry::Geometry(VertexState* vaoPtr, VertexBuffer *vboPtr, const geometry_t &geometry_type)
	: type(geometry_type), vertex_state(vaoPtr), vertex_buffer(vboPtr)
{
	if (this->type != geometry_t::quad) {
		throw std::invalid_argument("Only Quad geometry is currently supported");
	}
	this->setup_quad();
}

Geometry::~Geometry()
{

}

void Geometry::setup_quad()
{
	std::vector<float> data;
	for (const auto& vertex : this->quad_geometry) {
		data.push_back(vertex.position(0));
		data.push_back(vertex.position(1));
		data.push_back(vertex.texture_coordinates(0));
		data.push_back(vertex.texture_coordinates(1));
	}

	const size_t byte_size = data.size() * sizeof(decltype(data)::value_type);
	this->vertex_buffer->alloc(byte_size);
	auto buffer_data_ptr = this->vertex_buffer->get(true);
	std::memcpy(buffer_data_ptr, data.data(), byte_size);

	this->vertex_buffer->upload();

	VertexBuffer::vbo_section position_section{
		0,
		vertex_attribute_type::float_32,
		2u,
		0u,
		16u
	};
	VertexBuffer::vbo_section texture_coordinate_section{
		1,
		vertex_attribute_type::float_32,
		2u,
		8u,
		16u
	};

	this->vertex_buffer->add_section(position_section);
	this->vertex_buffer->add_section(texture_coordinate_section);
	this->vertex_state->attach_buffer(*(this->vertex_buffer));
}

}} //openage::renderer
