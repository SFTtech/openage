// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_GEOMETRY_H_
#define OPENAGE_RENDERER_GEOMETRY_H_

#include "vertex_buffer.h"
#include "vertex_state.h"

namespace openage {
namespace renderer {

class Context;
class Material;

/**
 * Drawable geometry, stores all triangles vertices.
 */
class Geometry {
public:
	Geometry(mesh_t vertices={});
	virtual ~Geometry() = default;

	/**
	 * Return the associated vertex list.
	 */
	const mesh_t &get_mesh();

protected:
	/**
	 * Triangle vertex storage.
	 */
	mesh_t vertices;
};

}} // openage::renderer

#endif
