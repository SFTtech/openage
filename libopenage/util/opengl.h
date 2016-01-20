// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#pragma once

namespace openage {
namespace util {

/**
 * query the current opengl context for any errors.
 *
 * raises exceptions on any error.
 */
void gl_check_error();

}
}
