#include "unicode.h"

namespace engine {
namespace util {

size_t utf8_decode(const unsigned char *s, size_t len, wchar_t *outbuf) {
	size_t advance;
	wchar_t w;
	size_t result = 0;

	while(len > 0) {
		if (s[0] < 0x80) {
			//1-byte (ASCII) character
			w = *s;
			advance = 1;
		} else if (len >= 2 && s[0] >= 0xc2 && s[0] <= 0xdf && (s[1] & 0xc0) == 0x80) {
			//2-byte character
			w = ((s[0] & 0x1f) << 6) | (s[1] & 0x3f);
			advance = 2;
		} else if (len >= 3 &&
			  ((s[0] == 0xe0  && s[1] >= 0xa0 && s[1] <= 0xbf)
			|| (s[0] >= 0xe1 && s[0] <= 0xec && s[1] >= 0x80 && s[1] <= 0xbf)
			|| (s[0] == 0xed && s[1] >= 0x80 && s[1] <= 0x9f)
			|| (s[0] >= 0xee && s[0] <= 0xef && s[1] >= 0x80 && s[1] <= 0xbf))
			&& (s[2] & 0xc0) == 0x80) {
			//3-byte character
			w = ((s[0] & 0x0f) << 12) | ((s[1] & 0x3f) << 6) | (s[2] & 0x3f);
			advance = 3;
		} else if (len >= 4 &&
			  ((s[0] == 0xf0 && s[1] >= 0x90 && s[1] <= 0xbf)
			|| (s[0] >= 0xf1 && s[0] <= 0xf3 && s[1] >= 0x80 && s[1] <= 0xbf)
			|| (s[0] == 0xf4 && s[1] >= 0x80 && s[1] <= 0x8f))
			&& (s[2] & 0xc0) == 0x80
			&& (s[3] & 0xc0) == 0x80) {
			//4-byte character
			w = ((s[0] & 0x07) << 18) | ((s[1] & 0x3f) << 12) | ((s[2] & 0x3f) << 6) | (s[3] & 0x3f);
			advance = 4;
		} else {
			//decoding error; try with next byte
			w = 0xfffd;
			advance = 1;
		}

		len -= advance;
		s += advance;

		*outbuf++ = w;
		result++;
	}

	return result;
}

} //namespace util
} //namespace engine
