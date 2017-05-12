// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "geometry.h"

#include <epoxy/gl.h>

#include "../../error/error.h"


namespace openage {
namespace renderer {
namespace opengl {

GlGeometry::GlGeometry()
	: Geometry(geometry_t::bufferless_quad) {}

void GlGeometry::draw() const {
	if (this->get_type() != geometry_t::bufferless_quad) {
		throw Error(MSG(err) << "Only drawing bufferless quads is currently supported.");
	}

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

}}} //openage::renderer::opengl
