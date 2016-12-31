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


TextureData::TextureData(int width, int height, char *data)
	:
	format{texture_format::rgba},
	w{width},
	h{height} {

	// rgba has 4 components
	size_t pixel_size = 4 * this->w * this->h;

	// copy pixel data from surface
	this->data = std::make_unique<uint8_t[]>(pixel_size);
	memcpy(this->data.get(), data, pixel_size);
}

TextureData::TextureData(int width, int height, std::unique_ptr<uint8_t[]> data)
	:
	format{texture_format::rgba},
	w{width},
	h{height} {

	this->data = std::move(data);
}


FileTextureData::FileTextureData(const std::string &filename,
                                 bool use_metafile)
	:
	use_metafile{use_metafile},
	filename{filename} {

	SDL_Surface *surface = IMG_Load(this->filename.c_str());

	if (!surface) {
		throw Error(MSG(err) <<
			"Could not load texture from " <<
			filename << ": " << IMG_GetError());
	} else {
		log::log(MSG(dbg) << "Texture has been loaded from " << filename);
	}

	switch (surface->format->BytesPerPixel) {
	case 3: // RGB 24 bit
		this->format = texture_format::rgb;
		break;
	case 4: // RGBA 32 bit
		this->format = texture_format::rgba;
		break;
	default:
		throw Error(
			MSG(err) << "Unknown texture bit depth for "
			<< filename << ": " << surface->format->BytesPerPixel
			<< " bytes per pixel");
	}

	this->w = surface->w;
	this->h = surface->h;

	size_t pixel_size = surface->format->BytesPerPixel * surface->w * surface->h;

	// copy pixel data from surface
	this->data = std::make_unique<uint8_t[]>(pixel_size);
	memcpy(this->data.get(), surface->pixels, pixel_size);
	SDL_FreeSurface(surface);

	if (use_metafile) {
		// change the suffix to .docx (lol)
		std::string meta_filename = this->filename;

		size_t start_pos = meta_filename.find_last_of(".");
		if (start_pos == std::string::npos) {
			throw Error(
				MSG(err) << "No filename extension found in: "
				<< meta_filename
			);
		}

		// TODO: will probably bug if previous extension is longer than 5
		meta_filename.replace(start_pos, 5, ".docx");

		log::log(MSG(info) << "Loading meta file: " << meta_filename);

		// get subtexture information by meta file exported by script
		util::read_csv_file<gamedata::subtexture>(meta_filename, this->subtextures);
	}
	else {
		// we don't have a texture description file.
		// use the whole image as one texture then.
		gamedata::subtexture s{0, 0, this->w, this->h, this->w/2, this->h/2};

		this->subtextures.push_back(s);
	}
}


}} // namespace openage::renderer
