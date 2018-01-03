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

/// Tries to guess the alignment of image rows based on image parameters. Kinda
/// black magic and might not actually work.
/// @param width in pixels of the image
/// @param fmt of pixels in the image
/// @param row_size the actual size in bytes of an image row, including padding
static size_t guess_row_alignment(size_t width, pixel_format fmt, size_t row_size) {
	// Use the highest possible alignment for even-width images.
	if (width % 8 == 0) {
		return 8;
	} else if (width % 4 == 0) {
		return 4;
	} else if (width % 2 == 0) {
		return 2;
	}

	// The size of meaningful data in each row.
	size_t pix_bytes = width * pixel_size(fmt);
	// The size of padding.
	size_t padding = row_size - pix_bytes;

	if (padding == 0) {
		return 1;
	} else if (padding <= 1) {
		return 2;
	} else if (padding <= 3) {
		return 4;
	} else if (padding <= 7) {
		return 8;
	}

	// Bail with a sane value.
	return 4;
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

	auto surf_fmt = *surface->format;

	pixel_format pix_fmt;
	switch (surf_fmt.format) {
	case SDL_PIXELFORMAT_RGB24:
			pix_fmt = pixel_format::rgb8;
			break;
	case SDL_PIXELFORMAT_BGR24:
			pix_fmt = pixel_format::bgr8;
			break;
	case SDL_PIXELFORMAT_RGBA32:
			pix_fmt = pixel_format::rgba8;
			break;
	default:
			throw Error(MSG(err) << "Texture " << native_path << " uses an unsupported format.");
	}

	auto w = surface->w;
	auto h = surface->h;

	size_t data_size = surf_fmt.BytesPerPixel * surface->w * surface->h;

	// copy pixel data from surface
	this->data = std::vector<uint8_t>(data_size);
	memcpy(this->data.data(), surface->pixels, data_size);
	SDL_FreeSurface(surface);

	std::vector<gamedata::subtexture> subtextures;
	if (use_metafile) {
		util::Path meta = path.get_parent() / path.get_stem() / ".docx";

		log::log(MSG(info) << "Loading meta file: " << meta);

		// get subtexture information by meta file exported by script
		subtextures = util::read_csv_file<gamedata::subtexture>(meta);
	}
	else {
		// we don't have a texture description file.
		// use the whole image as one texture then.
		gamedata::subtexture s{0, 0, w, h, w/2, h/2};

		subtextures.push_back(s);
	}

	size_t align = guess_row_alignment(w, pix_fmt, surface->pitch);
	this->info = TextureInfo(w, h, pix_fmt, align, std::move(subtextures));
}

TextureData::TextureData(TextureInfo &&info, std::vector<uint8_t> &&data)
	: info(std::move(info))
	, data(std::move(data)) {}

const TextureInfo& TextureData::get_info() const {
	return this->info;
}

const uint8_t *TextureData::get_data() const {
	return this->data.data();
}

void TextureData::store(const util::Path& file) const {
	log::log(MSG(info) << "Saving texture data to " << file);

	// TODO support row_alignment in copying below
	throw "unimplemented";
	if (this->info.get_format() != pixel_format::rgba8) {
		throw "unimplemented";
	}

	// color masks.
	int32_t rmask, gmask, bmask, amask;
	rmask = 0x000000FF;
	gmask = 0x0000FF00;
	bmask = 0x00FF0000;
	amask = 0xFF000000;

	auto size = this->info.get_size();

	// create output surface which will be stored later.
	SDL_Surface *screen = SDL_CreateRGBSurface(
		SDL_SWSURFACE,
		size.first, size.second,
		32, rmask, gmask, bmask, amask
	);

	uint32_t *surf_data = reinterpret_cast<uint32_t*>(screen->pixels);
	const uint32_t *data = reinterpret_cast<const uint32_t*>(this->data.data());

	// now copy the raw data to the sdl surface.
	// we need to invert all pixel rows, but leave column order the same.
	for (ssize_t row = 0; row < screen->h; row++) {
		ssize_t irow = screen->h - 1 - row;
		for (ssize_t col = 0; col < screen->w; col++) {
			uint32_t pixel = data[irow * screen->w + col];

			// TODO: store the alpha channels in the file,
			// is buggy at the moment..
			surf_data[row * screen->w + col] = pixel | 0xFF000000;
		}
	}

	// call sdl_image for saving the screenshot to png
	std::string path = file.resolve_native_path();
	IMG_SavePNG(screen, path.c_str());
	SDL_FreeSurface(screen);
}

}}}
