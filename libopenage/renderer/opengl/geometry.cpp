// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "geometry.h"

#include <epoxy/gl.h>

#include "../../error/error.h"
#include "../../datastructure/constexpr_map.h"


namespace openage {
namespace renderer {
namespace opengl {

static constexpr auto gl_prim = datastructure::create_const_map<resources::vertex_primitive_t, GLenum>(
	std::make_pair(resources::vertex_primitive_t::POINTS, GL_POINTS),
	std::make_pair(resources::vertex_primitive_t::LINES, GL_LINES),
	std::make_pair(resources::vertex_primitive_t::LINE_STRIP, GL_LINE_STRIP),
	std::make_pair(resources::vertex_primitive_t::TRIANGLES, GL_TRIANGLES),
	std::make_pair(resources::vertex_primitive_t::TRIANGLE_STRIP, GL_TRIANGLE_STRIP),
	std::make_pair(resources::vertex_primitive_t::TRIANGLE_FAN, GL_TRIANGLE_FAN)
);

static constexpr auto gl_idx_t = datastructure::create_const_map<resources::index_t, GLenum>(
	std::make_pair(resources::index_t::U8, GL_UNSIGNED_BYTE),
	std::make_pair(resources::index_t::U16, GL_UNSIGNED_SHORT),
	std::make_pair(resources::index_t::U32, GL_UNSIGNED_INT)
);

GlGeometry::GlGeometry()
	: Geometry(geometry_t::bufferless_quad) {}

GlGeometry::GlGeometry(const resources::MeshData &mesh)
	: Geometry(geometry_t::mesh) {
	GlBuffer verts(mesh.get_data().data(), mesh.get_data().size());

	this->mesh = GlMesh {
		std::move(verts),
		GlVertexArray (verts, mesh.get_info()),
		{},
		{},
		mesh.get_data().size() / mesh.get_info().vert_size(),
		gl_prim.get(mesh.get_info().get_primitive()),
	};

	if (mesh.get_ids()) {
		this->mesh->indices = GlBuffer(mesh.get_ids()->data(), mesh.get_ids()->size());
		this->mesh->index_type = gl_idx_t.get(*mesh.get_info().get_index_type());
		this->mesh->vert_count = mesh.get_ids()->size() / sizeof(GLuint);
	}
}

void GlGeometry::update_verts_offset(std::vector<uint8_t> const &verts, size_t offset) {
	if (this->get_type() != geometry_t::mesh) {
		throw Error(MSG(err) << "Cannot update vertex data for non-mesh GlGeometry.");
	}

	if (verts.size() != this->mesh->vertices.get_size()) {
		throw Error(MSG(err) << "Size mismatch between old and new vertex data for GlGeometry.");
	}

	// TODO support offset updating
	this->mesh->vertices.upload_data(verts.data(), offset, verts.size());
}

void GlGeometry::draw() const {
	if (this->get_type() == geometry_t::bufferless_quad) {
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	else if (this->get_type() == geometry_t::mesh) {
		auto const& mesh = *this->mesh;
		mesh.vao.bind();

		if (mesh.indices) {
			mesh.indices->bind(GL_ELEMENT_ARRAY_BUFFER);

			glDrawElements(mesh.primitive, mesh.vert_count, *mesh.index_type, 0);
		}
		else {
			glDrawArrays(GL_TRIANGLE_STRIP, 0, mesh.vert_count);
		}
	}
}

}}} //openage::renderer::opengl
