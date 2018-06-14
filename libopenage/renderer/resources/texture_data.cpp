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

Texture2dData::Texture2dData(const util::Path &path, bool use_metafile) {
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
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	case SDL_PIXELFORMAT_RGBA8888:
#else
	case SDL_PIXELFORMAT_ABGR8888:
#endif
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
		util::Path meta = (path.get_parent() / path.get_stem()).with_suffix(".slp.docx");
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
	this->info = Texture2dInfo(w, h, pix_fmt, align, std::move(subtextures));
}

Texture2dData::Texture2dData(Texture2dInfo &&info, std::vector<uint8_t> &&data)
	: info(std::move(info))
	, data(std::move(data)) {}

Texture2dData Texture2dData::flip_y() {
	size_t row_size = this->info.get_row_size();
	size_t height = this->info.get_size().second;

	std::vector<uint8_t> new_data(this->data.size());

	for (size_t y = 0; y < height; ++y) {
		std::copy(this->data.data() + row_size * y, this->data.data() + row_size * (y+1), new_data.end() - row_size * (y+1));
	}

	this->data = new_data;

	Texture2dInfo new_info(this->info);

	return Texture2dData(std::move(new_info), std::move(new_data));
}

const Texture2dInfo& Texture2dData::get_info() const {
	return this->info;
}

const uint8_t *Texture2dData::get_data() const {
	return this->data.data();
}

void Texture2dData::store(const util::Path& file) const {
	log::log(MSG(info) << "Saving texture data to " << file);

	if (this->info.get_format() != pixel_format::rgba8) {
		throw "unimplemented";
	}

	auto size = this->info.get_size();

// If an older SDL2 is used, we have to specify the format manually.
#ifndef SDL_PIXELFORMAT_RGBA32
	uint32_t rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else // little endian, like x86
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	SDL_Surface *surf = SDL_CreateRGBSurfaceFrom(
		// const_cast is okay, because the surface doesn't modify data
		const_cast<void*>(static_cast<void const*>(this->data.data())),
		size.first,
		size.second,
		32,
		this->info.get_row_size(),
		rmask, gmask, bmask, amask
	);
#else
	SDL_Surface *surf = SDL_CreateRGBSurfaceWithFormatFrom(
		// const_cast is okay, because the surface doesn't modify data
		const_cast<void*>(static_cast<void const*>(this->data.data())),
		size.first,
		size.second,
		32,
		this->info.get_row_size(),
		SDL_PIXELFORMAT_RGBA32
	);
#endif

	// Call sdl_image for saving the screenshot to PNG
	std::string path = file.resolve_native_path_w();
	IMG_SavePNG(surf, path.c_str());
	SDL_FreeSurface(surf);
}

}}}
