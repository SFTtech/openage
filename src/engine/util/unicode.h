#ifndef _ENGINE_UTIL_UNICODE_H_
#define _ENGINE_UTIL_UNICODE_H_

#include <stdint.h>
#include <stdlib.h>

namespace engine {
namespace util {

class utf8_decoder {
public:
	utf8_decoder();

	/**
	 * current result character code point;
	 * overwritten on feed().
	 *
	 * if no valid output character currently exists,
	 * this is negative or remaining is not 0.
	 */
	int32_t out;

	/**
	 * number of remaining characters
	 */
	unsigned remaining;

	/**
	 * feeds one char to the decoder state machine
	 * returns false on decoding error, true else
	 *
	 * once a character has been completely decoded,
	 * remaining is set to false and out is set to the
	 * decoded character.
	 * note that even if true is returned, remaining MAY
	 * be false (e.g. multi-byte characters),
	 * and remaining MAY be true even if false is returned
	 * (successful re-synchronization).
	 *
	 * thus, after each call to feed(), you'll first want
	 * to evaluate the return value and print an error
	 * message or add the U+FFFD replacement character.
	 * then, you'll want to check for
	 *   remaining == 0
	 *   out >= 0
	 */
	bool feed(char c);

	/**
	 * resets the decoder to its initial state
	 */
	void reset();
};

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
size_t utf8_decode(const unsigned char *s, size_t len, int32_t *outbuf);

} //namespace util
} //namespace engine

#endif //_ENGINE_UTIL_UNICODE_H_
