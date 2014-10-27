#ifndef OPENAGE_UTIL_OPENGL_H_B2FE47F293824607A2C6DD4B6B80ED69
#define OPENAGE_UTIL_OPENGL_H_B2FE47F293824607A2C6DD4B6B80ED69

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
