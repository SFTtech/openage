// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "geometry.h"

namespace openage {
namespace renderer {

Geometry::Geometry(mesh_t vertices)
	:
	vertices{vertices} {}

mesh_t get_mesh() {
	return this->vertices()
}

}} // openage::renderer

#endif
