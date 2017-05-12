// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "geometry.h"


namespace openage {
namespace renderer {

Geometry::Geometry(geometry_t type)
	: type(type) {}

geometry_t Geometry::get_type() const {
	return this->type;
}

}} //openage::renderer
