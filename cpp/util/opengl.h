// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_OPENGL_H_
#define OPENAGE_UTIL_OPENGL_H_

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

#endif
