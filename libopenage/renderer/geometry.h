// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once


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

protected:
	/// The default constructor makes a quad.
	explicit Geometry(geometry_t type);

private:
	geometry_t type;
};

}} // openage::renderer
