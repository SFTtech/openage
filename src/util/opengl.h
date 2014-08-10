#ifndef _OPENAGE_UTIL_OPENGL_H_
#define _OPENAGE_UTIL_OPENGL_H_

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

#endif //_OPENAGE_UTIL_OPENGL_H_
