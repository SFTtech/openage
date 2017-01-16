// Copyright 2014-2017 the openage authors. See copying.md for legal info.

#include "screenshot.h"
#include "util/strings.h"

#include <math.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <epoxy/gl.h>

#include "coord/window.h"
#include "log/log.h"
#include <ctime>

namespace openage {


ScreenshotManager::ScreenshotManager()
	:
	count{0} {
}


ScreenshotManager::~ScreenshotManager() {}


std::string ScreenshotManager::gen_next_filename() {

	std::time_t t = std::time(NULL);

	if (t == this->last_time) {
		this->count++;
	} else {
		this->count = 0;
		this->last_time = t;
	}

	// these two values (32) *must* be the same for safety reasons
	char timestamp[32];
	std::strftime(timestamp, 32, "%Y-%m-%d_%H-%M-%S", std::localtime(&t));

	return util::sformat("/tmp/openage_%s_%02d.png", timestamp, this->count);
}


void ScreenshotManager::save_screenshot() {
	std::string filename = this->gen_next_filename();

	log::log(MSG(info) << "Saving screenshot to " << filename);

	int32_t rmask, gmask, bmask, amask;
	rmask = 0x000000FF;
	gmask = 0x0000FF00;
	bmask = 0x00FF0000;
	amask = 0xFF000000;

	SDL_Surface *screen = SDL_CreateRGBSurface(
	SDL_SWSURFACE,
	this->window_size.x,
	this->window_size.y,
	32,
	rmask, gmask, bmask, amask);

	size_t pxcount = screen->w * screen->h;

	auto pxdata = std::make_unique<uint32_t[]>(pxcount);

	glReadPixels(0, 0,
	             this->window_size.x, this->window_size.y,
	             GL_RGBA, GL_UNSIGNED_BYTE, pxdata.get());

	uint32_t *surface_pxls = (uint32_t *)screen->pixels;

	// we need to invert all pixel rows, but leave column order the same.
	for (ssize_t row = 0; row < screen->h; row++) {
		ssize_t irow = screen->h - 1 - row;
		for (ssize_t col = 0; col < screen->w; col++) {
			uint32_t pxl = pxdata[irow * screen->w + col];

			// TODO: store the alpha channels in the screenshot, is buggy at the moment..
			surface_pxls[row * screen->w + col] = pxl | 0xFF000000;
		}
	}

	// call sdl_image for saving the screenshot to png
	IMG_SavePNG(screen, filename.c_str());
	SDL_FreeSurface(screen);
}


} // openage
