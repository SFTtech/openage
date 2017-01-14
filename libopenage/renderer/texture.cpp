// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include <cstring>

#include "texture.h"

#include "../error/error.h"
#include "../util/file.h"

namespace openage {
namespace renderer {

Texture::Texture(Context *ctx)
	:
	context{ctx} {
}

Texture::~Texture() {}

const std::tuple<size_t, size_t> Texture::get_size() const {
	return std::make_tuple(this->w, this->h);
}


const gamedata::subtexture *Texture::get_subtexture(size_t subid) const {
	if (subid < this->subtextures.size()) {
		return &this->subtextures[subid];
	}
	else {
		throw Error(MSG(err) << "Unknown subtexture requested: " << subid);
	}
}


const std::tuple<float, float, float, float> Texture::get_subtexture_coordinates(size_t subid) const {
	auto tx = this->get_subtexture(subid);
	return std::make_tuple(
		((float)tx->x)           / this->w,
		((float)(tx->x + tx->w)) / this->w,
		((float)tx->y)           / this->h,
		((float)(tx->y + tx->h)) / this->h
	);
}


int Texture::get_subtexture_count() const {
	return this->subtextures.size();
}


const std::tuple<int, int> Texture::get_subtexture_size(size_t subid) const {
	auto subtex = this->get_subtexture(subid);
	return std::make_tuple(subtex->w, subtex->h);
}





}} // namespace openage::renderer
