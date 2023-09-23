// Copyright 2014-2023 the openage authors. See copying.md for legal info.

#pragma once

namespace openage::renderer::opengl {

/**
 * query the current opengl context for any errors.
 *
 * raises exceptions on any error.
 */
void gl_check_error();

} // namespace openage::renderer::opengl
