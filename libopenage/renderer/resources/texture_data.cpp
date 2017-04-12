// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "texture_data.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../../log/log.h"
#include "../../error/error.h"
#include "../../util/csv.h"


namespace openage {
namespace renderer {
namespace resources {

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

TextureData::TextureData(const util::Path &path, bool use_metafile) {
	std::string native_path = path.resolve_native_path();
	SDL_Surface *surface = IMG_Load(native_path.c_str());

	if (!surface) {
		throw Error(MSG(err) <<
			"Could not load texture from " <<
			native_path << ": " << IMG_GetError());
	} else {
		log::log(MSG(dbg) << "Texture has been loaded from " << native_path);
	}

	auto fmt = *surface->format;

	if (fmt.Rmask != 0xf000 || fmt.Gmask != 0x0f00 || fmt.Bmask != 0x00f0) {
		throw Error(MSG(err) << "Texture " << native_path << " is not in RGB or RGBA format.");
	}

	auto& info = this->info;
	if (fmt.Amask == 0) {
		if (fmt.BytesPerPixel != 3) {
			throw Error(MSG(err) << "Texture " << native_path << " is in an unsupported RGB format.");
		}

		info.format = pixel_format::rgb8;
	}
	else {
		info.format = pixel_format::rgba8;
	}

	info.w = surface->w;
	info.h = surface->h;

	size_t data_size = surface->format->BytesPerPixel * surface->w * surface->h;

	// copy pixel data from surface
	this->data = std::vector<uint8_t>(data_size);
	memcpy(this->data.data(), surface->pixels, data_size);
	SDL_FreeSurface(surface);

	if (use_metafile) {
		util::Path meta = path.get_parent() / path.get_stem() / ".docx";

		log::log(MSG(info) << "Loading meta file: " << meta);

		// get subtexture information by meta file exported by script
		info.subtextures = util::read_csv_file<gamedata::subtexture>(meta);
	}
	else {
		// we don't have a texture description file.
		// use the whole image as one texture then.
		gamedata::subtexture s{0, 0, info.w, info.h, info.w/2, info.h/2};

		info.subtextures.push_back(s);
	}
}

const TextureInfo& TextureData::get_info() const {
	return this->info;
}

const uint8_t *TextureData::get_data() const {
	return this->data.data();
}

}}}


