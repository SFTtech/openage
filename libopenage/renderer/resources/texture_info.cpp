// Copyright 2017-2023 the openage authors. See copying.md for legal info.

#include "texture_info.h"

#include <eigen3/Eigen/Dense>

#include "error/error.h"
#include "log/message.h"


namespace openage::renderer::resources {

Texture2dInfo::Texture2dInfo(size_t width,
                             size_t height,
                             pixel_format fmt,
                             std::optional<util::Path> imagepath,
                             size_t row_alignment,
                             std::vector<Texture2dSubInfo> &&subs) :
	w(width),
	h(height),
	format{fmt},
	row_alignment{row_alignment},
	imagepath{imagepath},
	subtextures{std::move(subs)} {}

bool Texture2dInfo::operator==(Texture2dInfo const &other) {
	return other.w == this->w
	       and other.h == this->h
	       and other.format == this->format
	       and other.row_alignment == this->row_alignment;
}

bool Texture2dInfo::operator!=(Texture2dInfo const &other) {
	return not(*this == other);
}

std::pair<int32_t, int32_t> Texture2dInfo::get_size() const {
	return std::make_pair(this->w, this->h);
}

pixel_format Texture2dInfo::get_format() const {
	return this->format;
}

size_t Texture2dInfo::get_row_alignment() const {
	return this->row_alignment;
}

size_t Texture2dInfo::get_row_size() const {
	size_t px_size = pixel_size(this->format);
	size_t row_size = this->w * px_size;

	if (row_size % this->row_alignment != 0) {
		// Unaligned rows, have to add padding.
		size_t padding = this->row_alignment - (row_size % this->row_alignment);
		row_size += padding;
	}

	return row_size;
}

size_t Texture2dInfo::get_data_size() const {
	return this->get_row_size() * this->h;
}

const std::optional<util::Path> &Texture2dInfo::get_image_path() const {
	return this->imagepath;
}

size_t Texture2dInfo::get_subtex_count() const {
	return this->subtextures.size();
}

const Texture2dSubInfo &Texture2dInfo::get_subtex_info(size_t subidx) const {
	if (subidx < this->subtextures.size()) [[likely]] {
		return this->subtextures[subidx];
	}

	throw Error(MSG(err) << "Unknown subtexture requested: " << subidx);
}

std::tuple<float, float, float, float> Texture2dInfo::get_subtexture_coordinates(size_t subidx) const {
	auto tx = this->get_subtex_info(subidx);
	return std::make_tuple(
		(static_cast<float>(tx.get_pos()[0])) / this->w,
		(static_cast<float>(tx.get_pos()[0] + tx.get_size()[0])) / this->w,
		(static_cast<float>(tx.get_pos()[1])) / this->h,
		(static_cast<float>(tx.get_pos()[1] + tx.get_size()[1])) / this->h);
}

} // namespace openage::renderer::resources
