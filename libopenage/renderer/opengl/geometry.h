// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "../geometry.h"


namespace openage {
namespace renderer {
namespace opengl {

/// The OpenGL class representing geometry to be passed to a draw call.
class GlGeometry : public Geometry {
public:
	/// The default constructor makes a quad.
	GlGeometry(const geometry_t &geometry_type);
	virtual ~GlGeometry();

	virtual void draw() const override;
};

}}} // openage::renderer::opengl
