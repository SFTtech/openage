// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>

#include <epoxy/gl.h>

#include "renderer/opengl/lookup.h"


namespace openage::renderer::opengl {

/**
 * Get the sizes of a uniform value with a given uniform type.
 *
 * Guaranteed to be a evaluated at compile time.
 *
 * @param type Uniform type.
 *
 * @return Size of uniform value (in bytes).
 */
consteval size_t get_uniform_type_size(GLenum type) {
	return GL_UNIFORM_TYPE_SIZE.get(type);
}

} // namespace openage::renderer::opengl
