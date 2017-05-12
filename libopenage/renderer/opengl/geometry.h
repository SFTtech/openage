// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "../geometry.h"


namespace openage {
namespace renderer {
namespace opengl {

/// The OpenGL class representing geometry to be passed to a draw call.
class GlGeometry final : public Geometry {
public:
	/// The default constructor makes a quad.
	GlGeometry();
	virtual ~GlGeometry() = default;

	/// Executes a draw command for the geometry on the currently active context.
	/// Assumes bound and valid shader program and all other necessary state.
	void draw() const;
};

}}} // openage::renderer::opengl
