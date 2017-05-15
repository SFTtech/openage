// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "texture_info.h"

#include "../../datastructure/constexpr_map.h"


namespace openage {
namespace renderer {
namespace resources {

static constexpr auto pix_size = datastructure::create_const_map<pixel_format, size_t>(
	std::make_pair(pixel_format::r16ui, 2),
	std::make_pair(pixel_format::r32ui, 4),
	std::make_pair(pixel_format::rgb8, 3),
	std::make_pair(pixel_format::depth24, 3),
	std::make_pair(pixel_format::rgba8, 4),
	std::make_pair(pixel_format::rgba8ui, 4)
);

size_t pixel_size(pixel_format fmt) {
	return pix_size.get(fmt);
}

TextureInfo::TextureInfo(size_t width, size_t height, pixel_format fmt, size_t row_alignment, std::vector<gamedata::subtexture> &&subs)
	: w(width)
	, h(height)
	, format(fmt)
	, row_alignment(row_alignment)
	, subtextures(std::move(subs)) {}

std::pair<int32_t, int32_t> TextureInfo::get_size() const {
	return std::make_pair(this->w, this->h);
}

pixel_format TextureInfo::get_format() const {
	return this->format;
}

size_t TextureInfo::get_row_alignment() const {
	return this->row_alignment;
}

size_t TextureInfo::get_row_size() const {
	size_t px_size = pixel_size(this->format);
	size_t row_size = this->w * px_size;
	row_size += row_size % this->row_alignment; // there might be padding at row ends to match the alignment
	return row_size;
}

size_t TextureInfo::get_data_size() const {
	return this->get_row_size() * this->h;
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

std::pair<int, int> TextureInfo::get_subtexture_size(size_t subid) const {
	auto subtex = this->get_subtexture(subid);
	return std::make_pair(subtex.w, subtex.h);
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

}}}
