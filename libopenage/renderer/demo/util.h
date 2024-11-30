// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>

#include "renderer/renderable.h"

namespace openage::renderer::tests {

// Macro for debugging OpenGL state.
#define DBG_GL(txt) \
	printf("before %s\n", txt); \
	opengl::GlContext::check_error(); \
	printf("after %s\n", txt);

/**
 * Check if all uniform values for the given renderables have been set.
 *
 * @param renderables The list of renderable objects to check.
 * @return true if all uniforms have been set, false otherwise.
 */
bool check_uniform_completeness(const std::vector<Renderable> &renderables);

} // namespace openage::renderer::tests
