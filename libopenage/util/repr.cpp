// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "repr.h"

namespace openage {
namespace util {


constexpr const char *HEX_DIGITS = "0123456789abcdef";


std::string repr(const std::string &arg) {
	std::string result;
	result.push_back('"');
	for (char c : arg) {
		if (c >= 0x20 && c < 0x7f) {
			// printable
			if (c == '\\' || c == '"') {
				// but it must be escaped
				result.push_back('\\');
			}
			result.push_back(c);
		} else {
			// non-printable
			result.push_back('\\');
			result.push_back('x');
			result.push_back(HEX_DIGITS[(c & 0xf0) >> 4]);
			result.push_back(HEX_DIGITS[(c & 0x0f) >> 0]);
		}
	}
	result.push_back('"');
}


}} // namespace openage::util
