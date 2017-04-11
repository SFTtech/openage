// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "vertex_buffer.h"
#include "vertex_state.h"

namespace openage {
namespace renderer {

enum class geometry_t {
	quad,
	mesh,
};

class Geometry {
public:
	/// The default constructor makes a quad.
	Geometry() : type(geometry_t::quad) {}

	// Geometry(mesh..)

	geometry_t get_type() const { return this->type; }

private:
	geometry_t type;
};

}} // openage::renderer
