// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once


namespace openage {
namespace renderer {

/// The type of geometry.
enum class geometry_t {
	quad,
	mesh,
};

/// A class representing geometry to be passed to a draw call.
// TODO this class
class Geometry {
public:
	/// The default constructor makes a quad.
	Geometry() : type(geometry_t::quad) {}

	// TODO maybe use inheritance instead of enum?
	// Geometry(mesh..)

	/// Returns the type of this geometry.
	geometry_t get_type() const { return this->type; }

private:
	geometry_t type;
};

}} // openage::renderer
