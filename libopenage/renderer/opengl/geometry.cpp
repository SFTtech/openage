// Copyright 2015-2025 the openage authors. See copying.md for legal info.

#include "geometry.h"

#include <epoxy/gl.h>

#include "../../datastructure/constexpr_map.h"
#include "../../error/error.h"

#include "lookup.h"


namespace openage {
namespace renderer {
namespace opengl {

GlGeometry::GlGeometry() :
	Geometry(geometry_t::bufferless_quad) {}

GlGeometry::GlGeometry(const std::shared_ptr<GlContext> &context, const resources::MeshData &mesh) :
	Geometry(geometry_t::mesh) {
	GlBuffer verts_buf{context, mesh.get_data().data(), mesh.get_data().size()};
	GlVertexArray verts_array(context, verts_buf, mesh.get_info());

	this->mesh = GlMesh{
		std::move(verts_buf),
		std::move(verts_array),
		{},
		{},
		mesh.get_data().size() / mesh.get_info().vert_size(),
		GL_PRIMITIVE.get(mesh.get_info().get_primitive()),
	};

	if (mesh.get_ids()) {
		this->mesh->indices = GlBuffer{context, mesh.get_ids()->data(), mesh.get_ids()->size()};
		this->mesh->index_type = GL_INDEX_TYPE.get(*mesh.get_info().get_index_type());
		this->mesh->vert_count = mesh.get_ids()->size() / GL_INDEX_SIZE.get(*mesh.get_info().get_index_type());
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
	switch (this->get_type()) {
	case geometry_t::bufferless_quad:
		// any VAO must be bound before this call
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		break;

	case geometry_t::mesh: {
		auto const &mesh = *this->mesh;
		mesh.vao.bind();

		if (mesh.indices) {
			// TODO: Binding the EBO may not be necessary if the VAO is already bound.
			mesh.indices->bind(GL_ELEMENT_ARRAY_BUFFER);

			glDrawElements(mesh.primitive, mesh.vert_count, *mesh.index_type, nullptr);
		}
		else {
			glDrawArrays(mesh.primitive, 0, mesh.vert_count);
		}

		break;
	}
	default:
		throw Error(MSG(err) << "Unknown geometry type in GlGeometry.");
	}
}

} // namespace opengl
} // namespace renderer
} // namespace openage
