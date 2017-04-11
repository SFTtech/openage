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

TextureData::TextureData(const char *filename, bool use_metafile) {
	SDL_Surface *surface = IMG_Load(filename);

	if (!surface) {
		throw Error(MSG(err) <<
			"Could not load texture from " <<
			filename << ": " << IMG_GetError());
	} else {
		log::log(MSG(dbg) << "Texture has been loaded from " << filename);
	}

	auto fmt = *surface->format;

	if (fmt.Rmask != 0xf000 || fmt.Gmask != 0x0f00 || fmt.Bmask != 0x00f0) {
		throw Error(MSG(err) << "Texture " << filename << " is not in RGB or RGBA format.");
	}

	auto& info = this->info;
	if (fmt.Amask == 0) {
		if (fmt.BytesPerPixel != 3) {
			throw Error(MSG(err) << "Texture " << filename << " is in an unsupported RGB format.");
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
		// change the suffix to .docx (lol)
		std::string meta_filename(filename);

		size_t start_pos = meta_filename.find_last_of(".");
		if (start_pos == std::string::npos) {
			throw Error(
				MSG(err) << "No filename extension found in: "
				<< meta_filename
			);
		}

		meta_filename.replace(start_pos, 5, ".docx");
		meta_filename = meta_filename.substr(0, start_pos + 5);

		log::log(MSG(info) << "Loading meta file: " << meta_filename);

		// get subtexture information by meta file exported by script
		// TODO wot
		//util::File file(meta_filename);
		//info.subtextures = util::read_csv_file<gamedata::subtexture>(file);
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


