// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#include "unicode.h"

namespace openage {
namespace util {

utf8_decoder::utf8_decoder() {
	reset();
}

void utf8_decoder::reset() {
	out = -1;
	remaining = 0;
}

bool utf8_decoder::feed(char c) {
	bool result = true;

	if ((c & (1 << 7)) == 0) {
		// single-byte character
		if (remaining) {
			// successful re-synchronization
			// (current multi-byte character discarded)
			result = false;
		}

		remaining = 0;
		out = c;
	} else if (c & (1 << 6)) {
		if (remaining) {
			// successful re-synchronization
			// (current multi-byte character discarded)
			result = false;
		}

		// beginning of a multi-byte character
		if ((c & (1 << 5)) == 0) {
			// 2-byte character
			remaining = 1;
			out = c & ((1 << 5) - 1);
		} else if ((c & (1 << 4)) == 0) {
			// 3-byte character
			remaining = 2;
			out = c & ((1 << 4) - 1);
		} else if ((c & (1 << 3)) == 0) {
			// 4-byte character
			remaining = 3;
			out = c & ((1 << 3) - 1);
		} else {
			// no 5- or 6-byte characters exist in utf8
			remaining = 0;
			out = -1;
			result = false;
		}
	} else {
		// inside a multi-byte character
		if (!remaining) {
			// we expected the start of a new character
			result = false;
		}

		remaining -= 1;
		out = (out << 6) | (c & ((1 << 6) - 1));
	}

	return result;
}

size_t utf8_decode(const unsigned char *s, size_t len, int32_t *outbuf) {
	size_t advance;
	wchar_t w;
	size_t result = 0;

	while(len > 0) {
		if (s[0] < 0x80) {
			// 1-byte (ASCII) character
			w = *s;
			advance = 1;
		} else if (len >= 2 && s[0] >= 0xc2 && s[0] <= 0xdf && (s[1] & 0xc0) == 0x80) {
			// 2-byte character
			w = ((s[0] & 0x1f) << 6) | (s[1] & 0x3f);
			advance = 2;
		} else if (len >= 3 &&
			  ((s[0] == 0xe0  && s[1] >= 0xa0 && s[1] <= 0xbf)
			|| (s[0] >= 0xe1 && s[0] <= 0xec && s[1] >= 0x80 && s[1] <= 0xbf)
			|| (s[0] == 0xed && s[1] >= 0x80 && s[1] <= 0x9f)
			|| (s[0] >= 0xee && s[0] <= 0xef && s[1] >= 0x80 && s[1] <= 0xbf))
			&& (s[2] & 0xc0) == 0x80) {
			// 3-byte character
			w = ((s[0] & 0x0f) << 12) | ((s[1] & 0x3f) << 6) | (s[2] & 0x3f);
			advance = 3;
		} else if (len >= 4 &&
			  ((s[0] == 0xf0 && s[1] >= 0x90 && s[1] <= 0xbf)
			|| (s[0] >= 0xf1 && s[0] <= 0xf3 && s[1] >= 0x80 && s[1] <= 0xbf)
			|| (s[0] == 0xf4 && s[1] >= 0x80 && s[1] <= 0x8f))
			&& (s[2] & 0xc0) == 0x80
			&& (s[3] & 0xc0) == 0x80) {
			// 4-byte character
			w = ((s[0] & 0x07) << 18) | ((s[1] & 0x3f) << 12) | ((s[2] & 0x3f) << 6) | (s[3] & 0x3f);
			advance = 4;
		} else {
			// decoding error; try with next byte
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

size_t utf8_encode(int cp, char *outbuf) {
	if (cp < 0) {
		// illegal codepoint (negative)
		outbuf[0] = '\0';
		return 0;
	} else if (cp < 0x80) {
		outbuf[0] = cp;
		outbuf[1] = '\0';
		return 1;
	} else if (cp < 0x800) {
		outbuf[2] = '\0';
		outbuf[1] = 0x80 | (cp & 0x3f); cp >>= 6;
		outbuf[0] = 0xc0 | cp;
		return 2;
	} else if (cp < 0x10000) {
		outbuf[3] = '\0';
		outbuf[2] = 0x80 | (cp & 0x3f); cp >>= 6;
		outbuf[1] = 0x80 | (cp & 0x3f); cp >>= 6;
		outbuf[0] = 0xe0 | cp;
		return 3;
	} else if (cp < 0x200000) {
		outbuf[4] = '\0';
		outbuf[3] = 0x80 | (cp & 0x3f); cp >>= 6;
		outbuf[2] = 0x80 | (cp & 0x3f); cp >>= 6;
		outbuf[1] = 0x80 | (cp & 0x3f); cp >>= 6;
		outbuf[0] = 0xf0 | cp;
		return 4;
	} else {
		// illegal codepoint: unicode is only defined up to 0x1fffff
		outbuf[0] = '\0';
		return 0;
	}
}

}} // openage::util
