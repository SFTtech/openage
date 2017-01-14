// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "texture_data.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../../log/log.h"
#include "../../error/error.h"
#include "../../util/file.h"


namespace openage {
namespace renderer {
namespace resources {

TextureData TextureData::load_from_file(std::experimental::string_view filename, bool use_metafile) {
	SDL_Surface *surface = IMG_Load(filename.data());

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

	pixel_format format;
	if (fmt.Amask == 0) {
		if (fmt.BytesPerPixel != 3) {
			throw Error(MSG(err) << "Texture " << filename << " is in an unsupported RGB format.");
		}

		format = pixel_format::rgb8;
	}
	else {
		format = pixel_format::rgba8;
	}

	auto w = surface->w;
	auto h = surface->h;

	size_t data_size = surface->format->BytesPerPixel * surface->w * surface->h;

	// copy pixel data from surface
	std::vector<uint8_t> data(data_size);
	memcpy(data.data(), surface->pixels, data_size);
	SDL_FreeSurface(surface);

	std::vector<gamedata::subtexture> subtextures;
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
		util::read_csv_file<gamedata::subtexture>(meta_filename, subtextures);
	}
	else {
		// we don't have a texture description file.
		// use the whole image as one texture then.
		gamedata::subtexture s{0, 0, w, h, w/2, h/2};

		subtextures.push_back(s);
	}

	return TextureData(w, h, format, std::move(data), std::move(subtextures));
}

TextureData::TextureData(uint32_t width, uint32_t height, pixel_format fmt, uint8_t *data, std::vector<gamedata::subtexture> &&subs)
	: format{fmt}
	, w{width}
	, h{height}
	, subtextures(std::move(subs))
{
		size_t data_size = this->w * this->h;
		switch (format) {
		case pixel_format::rgb8:
			data_size *= 3;
			break;
		case pixel_format::rgba8:
			data_size *= 4;
			break;
		default:
			throw Error(MSG(err) << "Unknown pixel format.");
		}

		this->data = std::vector<uint8_t>(data_size);
		memcpy(this->data.data(), data, data_size);
}

TextureData::TextureData(uint32_t width, uint32_t height, pixel_format fmt, std::vector<uint8_t> &&data, std::vector<gamedata::subtexture> &&subs)
	: format{fmt}
	, w{width}
	, h{height}
	, data(std::move(data))
	, subtextures(std::move(subs)) {}

}}}
