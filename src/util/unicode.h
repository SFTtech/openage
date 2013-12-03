#ifndef _UTIL_UNICODE_H_
#define _UTIL_UNICODE_H_

#include <stdint.h>
#include <stdlib.h>

namespace openage {
namespace util {

/**
 * decodes a UTF-8 character string of given length
 *
 * the results are written to outbuf.
 * no leading 0 is written to outbuf (nor is one expected at s).
 * outbuf MUST be large enough to hold all characters.
 * to ensure that outbuf is large enough, it SHOULD have a size of at least len.
 *
 * the number of actual unicode characters is returned.
 * it might be anywhere in the range [len/4; len].
 *
 * in case of decoding errors, the special unicode character 0xfffd is written.
 *
 * code logic gratefully borrowed from rxvt-unicode.
 */
size_t utf8_decode(const unsigned char *s, size_t len, wchar_t *outbuf);

} //namespace util
} //namespace openage

#endif //_UTIL_UNICODE_H_
