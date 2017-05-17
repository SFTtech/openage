// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <experimental/optional>

#include "../geometry.h"
#include "../resources/mesh_data.h"

#include "buffer.h"
#include "vertex_array.h"


namespace openage {
namespace renderer {
namespace opengl {

/// The OpenGL class representing geometry to be passed to a draw call.
class GlGeometry final : public Geometry {
public:
	/// The default constructor makes a quad.
	GlGeometry();

	/// Initialize a meshed geometry. Relatively costly, has to initialize GL buffers and copy vertex data.
	explicit GlGeometry(resources::MeshData const&);

	/// Executes a draw command for the geometry on the currently active context.
	/// Assumes bound and valid shader program and all other necessary state.
	void draw() const;

private:
	/// All the pieces of OpenGL state that represent a mesh.
	struct GlMesh {
		GlBuffer vertices;
		GlVertexArray vao;
		std::experimental::optional<GlBuffer> indices;
		std::experimental::optional<GLenum> index_type;
		size_t vert_count;
		GLenum primitive;
	};

	/// Data managing GPU memory and interpretation of mesh data.
	/// Only present if the type is a mesh.
	std::experimental::optional<GlMesh> mesh;
};

}}} // openage::renderer::opengl
