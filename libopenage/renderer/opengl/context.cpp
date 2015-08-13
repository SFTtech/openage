// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "../../config.h"
#if WITH_OPENGL

#include "context.h"

#include <epoxy/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "program.h"
#include "texture.h"
#include "../../log/log.h"
#include "../../error/error.h"

namespace openage {
namespace renderer {
namespace opengl {

// TODO: get max available gl version
constexpr int opengl_version_major = 3;
constexpr int opengl_version_minor = 3;

Context::Context()
	:
	renderer::Context{context_type::opengl} {}

Context::~Context() {}

uint32_t Context::get_window_flags() const {
	return SDL_WINDOW_OPENGL;
}

void Context::prepare() {
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, opengl_version_major);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, opengl_version_minor);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
}

void Context::create(SDL_Window *window) {
	this->glcontext = SDL_GL_CreateContext(window);

	if (this->glcontext == nullptr) {
		throw Error(MSG(err) << "Failed creating OpenGL context: " << SDL_GetError());
	}

	// check the OpenGL version, for shaders n stuff
	int epoxy_glv =  opengl_version_major * 10 + opengl_version_minor;
	if (not epoxy_is_desktop_gl() or epoxy_gl_version() < epoxy_glv) {
		throw Error(MSG(err) << "OpenGL "
		                     << opengl_version_major << "." << opengl_version_minor
		                     << " not available");
	}

	log::log(MSG(info) << "Using OpenGL " << opengl_version_major << "." << opengl_version_minor);
}

void Context::setup() {
	// to quote the standard doc: 'The value gives a rough estimate of the
	// largest texture that the GL can handle'
	// -> wat?  anyways, we need at least 1024x1024.
	int max_texture_size;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
	log::log(MSG(dbg) << "Maximum supported texture size: " << max_texture_size);
	if (max_texture_size < 1024) {
		throw Error(MSG(err) << "Maximum supported texture size too small: " << max_texture_size);
	}

	int max_texture_units;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_texture_units);
	log::log(MSG(dbg) << "Maximum supported texture units: " << max_texture_units);
	if (max_texture_units < 2) {
		throw Error(MSG(err) << "Your GPU has not enough texture units: " << max_texture_units);
	}

	// vsync on
	SDL_GL_SetSwapInterval(1);

	// TODO: transform the following to this::set_feature

	// enable alpha blending
	glEnable(GL_BLEND);

	// order of drawing relevant for depth
	// what gets drawn last is displayed on top.
	glDisable(GL_DEPTH_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Context::destroy() {
	SDL_GL_DeleteContext(this->glcontext);
}


void Context::set_feature(context_feature feature, bool on) {
	// what feature to change? this is the argument to glEnable and glDisable.
	GLenum what;

	switch (feature) {
	case context_feature::blending:
		what = GL_BLEND;
		break;

	case context_feature::depth_test:
		what = GL_DEPTH_TEST;
		break;

	default:
		throw Error(MSG(err) << "unknown opengl context feature to set");
	}

	if (on) {
		glEnable(what);
	} else {
		glDisable(what);
	}
}


void Context::screenshot(const std::string &filename) {
	log::log(MSG(info) << "Saving screenshot to " << filename);

	// surface color masks.
	int32_t rmask, gmask, bmask, amask;
	rmask = 0x000000FF;
	gmask = 0x0000FF00;
	bmask = 0x00FF0000;
	amask = 0xFF000000;

	// create output surface which will be stored later.
	SDL_Surface *screen = SDL_CreateRGBSurface(
		SDL_SWSURFACE,
		this->canvas_size.x, this->canvas_size.y,
		32, rmask, gmask, bmask, amask
	);

	size_t pxcount = screen->w * screen->h;

	auto pxdata = std::make_unique<uint32_t[]>(pxcount);

	// copy the whole framebuffer to our local buffer.
	glReadPixels(0, 0,
	             this->canvas_size.x, this->canvas_size.y,
	             GL_RGBA, GL_UNSIGNED_BYTE, pxdata.get());

	uint32_t *surface_pxls = reinterpret_cast<uint32_t *>(screen->pixels);

	// now copy the raw data to the sdl surface.
	// we need to invert all pixel rows, but leave column order the same.
	for (ssize_t row = 0; row < screen->h; row++) {
		ssize_t irow = screen->h - 1 - row;
		for (ssize_t col = 0; col < screen->w; col++) {
			uint32_t pxl = pxdata[irow * screen->w + col];

			// TODO: store the alpha channels in the screenshot,
			// is buggy at the moment..
			surface_pxls[row * screen->w + col] = pxl | 0xFF000000;
		}
	}

	// call sdl_image for saving the screenshot to png
	IMG_SavePNG(screen, filename.c_str());
	SDL_FreeSurface(screen);
}


std::unique_ptr<renderer::Texture> Context::register_texture(const TextureData &data) {
	std::unique_ptr<renderer::Texture> txt = std::make_unique<opengl::Texture>(data);
	return txt;
}

std::unique_ptr<RawProgram> Context::register_program(const ProgramSource &data) {
	std::unique_ptr<RawProgram> txt = std::make_unique<opengl::Program>(data);
	return txt;
}

void Context::resize_canvas(const coord::window &new_size) {
	log::log(MSG(dbg) << "opengl viewport resize to " << new_size.x << "x" << new_size.y);

	glViewport(0, 0, new_size.x, new_size.y);
}


}}} // namespace openage::renderer::opengl

#endif // if WITH_OPENGL
