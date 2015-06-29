// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_TASK_H_
#define OPENAGE_RENDERER_TASK_H_

#include "material.h"

namespace openage {
namespace renderer {

/**
 * render layer, their order is important.
 * layers from bottom to top: later in enum = drawn later
 */
enum class layer {
	terrain,
	unit,
	sky,
	hud,
};

/**
 * struct to submit to the renderer
 */
struct Task {
	layer layer;
	std::vector<Material> materials;
};


}} // namespace openage::renderer

#endif
