// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>
#include <cstdint>
#include <cstddef>


namespace openage {
namespace renderer {

/// The type of geometry.
enum class geometry_t {
	/// This passes 4 vertices with undefined positions to the shader.
	/// The shader has to set the positions itself (e.g. using gl_VertexID in OpenGL).
	bufferless_quad,
	/// This passes valid geometry defined by a mesh to the shader.
	mesh,
};

/// A class representing geometry to be passed to a draw call.
class Geometry {
public:
	virtual ~Geometry() = default;

	/// Returns the type of this geometry.
	geometry_t get_type() const;

	/// In a meshed geometry, updates the vertex data. The size and type of the vertex data has to be the same as before.
	/// If the mesh is indexed, indices will stay the same.
	/// @throws if there is a size mismatch between the new and old vertex data
	void update_verts(std::vector<uint8_t> const& verts);

	/// In a meshed geometry, updates the vertex data starting from the offset-th vertex. The type of the vertex
	/// data has to be the same as it was on initializing the geometry. The size plus the offset cannot exceed the
	/// previous size of the vertex data. If the mesh is indexed, indices will stay the same.
	/// @throws if there is a size mismatch between the new and old vertex data
	virtual void update_verts_offset(std::vector<uint8_t> const& verts, size_t offset) = 0;

protected:
	/// Initialize the geometry to a given type.
	explicit Geometry(geometry_t type);

private:
	geometry_t type;
};

}} // openage::renderer
