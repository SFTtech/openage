// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "texture_info.h"


namespace openage {
namespace renderer {
namespace resources {

TextureInfo::TextureInfo(size_t width, size_t height, pixel_format fmt, std::vector<gamedata::subtexture> &&subs)
	: w(width)
	, h(height)
	, format(fmt)
	, subtextures(std::move(subs)) {}

pixel_format TextureInfo::get_format() const {
	return this->format;
}

std::pair<int32_t, int32_t> TextureInfo::get_size() const {
	return std::make_pair(this->w, this->h);
}

size_t TextureInfo::get_subtexture_count() const {
	return this->subtextures.size();
}

const gamedata::subtexture& TextureInfo::get_subtexture(size_t subid) const {
	if (subid < this->subtextures.size()) {
		return this->subtextures[subid];
	}
	else {
		throw Error(MSG(err) << "Unknown subtexture requested: " << subid);
	}
}

std::tuple<float, float, float, float> TextureInfo::get_subtexture_coordinates(size_t subid) const {
	auto tx = this->get_subtexture(subid);
	return std::make_tuple(
		((float)tx.x)           / this->w,
		((float)(tx.x + tx.w)) / this->w,
		((float)tx.y)           / this->h,
		((float)(tx.y + tx.h)) / this->h
	);
}

std::pair<int, int> TextureInfo::get_subtexture_size(size_t subid) const {
	auto subtex = this->get_subtexture(subid);
	return std::make_pair(subtex.w, subtex.h);
}

}}}
