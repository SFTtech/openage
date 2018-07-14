// Copyright 2013-2018 the openage authors. See copying.md for legal info.

#include "texture_subinfo.h"


namespace openage {
namespace renderer {
namespace resources {

Texture2dSubInfo::Texture2dSubInfo(const std::string& line) {
	constexpr size_t member_count = 6;

	std::vector<std::string> buf = openage::util::split_escape(
		line, ',', member_count
	);

	if (buf.size() != member_count) {
		throw Error(
			MSG(err) << "Tokenizing subtexture led to "
			<< buf.size() << " columns (expected "
			<< member_count << ")!"
		);
	}

	size_t elems_read = 0;
	elems_read += sscanf(buf[0].c_str(), "%u", &this->x);
	elems_read += sscanf(buf[1].c_str(), "%u", &this->y);
	elems_read += sscanf(buf[2].c_str(), "%u", &this->w);
	elems_read += sscanf(buf[3].c_str(), "%u", &this->h);
	elems_read += sscanf(buf[4].c_str(), "%u", &this->cx);
	elems_read += sscanf(buf[5].c_str(), "%u", &this->cy);

	if (elems_read != member_count) {
		throw Error(MSG(err) << "Failed to read subtexture members from CSV line.");
	}
}

Texture2dSubInfo::Texture2dSubInfo(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t cx, uint32_t cy)
	: x(x)
	, y(y)
	, w(w)
	, h(h)
	, cx(cx)
	, cy(cy) {}

int Texture2dSubInfo::fill(const std::string& line) {
	*this = Texture2dSubInfo(line);
	return -1;
}

}}}
