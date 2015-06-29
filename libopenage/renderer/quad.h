// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_QUAD_H_
#define OPENAGE_RENDERER_QUAD_H_

#include "../coord/window.h"

namespace openage {
namespace renderer {

/**
 * struct to submit to the renderer
 */
struct Quad {
	coord::window vertices[4];
};


}} // namespace openage::renderer

#endif
