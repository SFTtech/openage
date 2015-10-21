// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_GEOMETRY_H_
#define OPENAGE_RENDERER_GEOMETRY_H_

#include "vertex_buffer.h"

namespace openage {
namespace renderer {

class Context;
class Material;

/**
 * Drawable geometry, initialized by a pipeline.
 */
class Geometry {
public:
	Geometry(Context *context, Material *material);
	Geometry(Context *context, Material *material, mesh_t tri_vertices);
	virtual ~Geometry();

	/**
	 * Create a render task from this geometry.
	 * This is then submitted to the renderer.
	 */
	Task get_task();

protected:
	Context *const context;
	Material *material;

	VertexBuffer vbuf;
};

}} // openage::renderer

#endif
