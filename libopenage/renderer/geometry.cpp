// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#include "geometry.h"


namespace openage {
namespace renderer {

Geometry::Geometry(geometry_t type)
	: type(type) {}

geometry_t Geometry::get_type() const {
	return this->type;
}

void Geometry::update_verts(const std::vector<uint8_t>& verts) {
	this->update_verts_offset(verts, 0);
}

}} //openage::renderer
