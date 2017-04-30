// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include <epoxy/gl.h>

#include "geometry.h"
#include "vertex_state.h"
#include "buffer.h"
#include "../vertex_buffer.h"

namespace openage {
namespace renderer {
namespace opengl {

GlGeometry::GlGeometry(const geometry_t &geometry_type)
	: Geometry(new VertexState(), new VertexBuffer(new Buffer(), Buffer::usage::static_draw), geometry_type)
{

}

GlGeometry::~GlGeometry()
{

}

void GlGeometry::draw() const
{
	if (this->get_type() != geometry_t::quad) {
		throw std::runtime_error("Only drawing quads is currently supported");
	}
	this->vertex_state->bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

}}} //openage::renderer::opengl
