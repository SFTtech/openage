// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>

#include <epoxy/gl.h>


namespace openage::renderer::opengl {

/**
 * Get the sizes of a uniform value with a given uniform type.
 *
 * @param type Uniform type.
 *
 * @return Size of uniform value (in bytes).
 */
size_t get_uniform_type_size(GLenum type);

} // namespace openage::renderer::opengl
