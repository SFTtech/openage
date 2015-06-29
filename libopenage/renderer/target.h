// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_TARGET_H_
#define OPENAGE_RENDERER_TARGET_H_

#include <vector>

#include "renderer.h"

namespace openage {
namespace renderer {

/**
 * Inherit from this to have methods that the renderer
 * uses to fetch render tasks.
 */
class Target {
	std::vector<task> get_render_tasks();
};

}} // namespace openage::renderer

#endif
