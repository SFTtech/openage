// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "../../config.h"
#if WITH_OPENGL

#include "context.h"

#include <epoxy/gl.h>
#include <SDL2/SDL.h>

#include "../../log/log.h"
#include "../../util/error.h"

namespace openage {
namespace renderer {
namespace opengl {

// TODO: get max available gl version
constexpr int opengl_version_major = 2;
constexpr int opengl_version_minor = 1;

Context::Context() {}
Context::~Context() {}

uint32_t Context::get_window_flags() {
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
		throw util::Error(MSG(err) << "Failed creating OpenGL context: " << SDL_GetError());
	}

	// check the OpenGL version, for shaders n stuff
	int epoxy_glv =  opengl_version_major * 10 + opengl_version_minor;
	if (not epoxy_is_desktop_gl() or epoxy_gl_version() < epoxy_glv) {
		throw util::Error(MSG(err) << "OpenGL "
		                           << opengl_version_major << "." << opengl_version_minor
		                           << " not available");
	}
}

void Context::setup() {
	// to quote the standard doc: 'The value gives a rough estimate of the
	// largest texture that the GL can handle'
	// -> wat?  anyways, we need at least 1024x1024.
	int max_texture_size;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
	log::log(MSG(dbg) << "Maximum supported texture size: " << max_texture_size);
	if (max_texture_size < 1024) {
		throw util::Error(MSG(err) << "Maximum supported texture size too small: " << max_texture_size);
	}

	int max_texture_units;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_texture_units);
	log::log(MSG(dbg) << "Maximum supported texture units: " << max_texture_units);
	if (max_texture_units < 2) {
		throw util::Error(MSG(err) << "Your GPU has not enough texture units: " << max_texture_units);
	}

	// vsync on
	SDL_GL_SetSwapInterval(1);

	// TODO: move the following statements to some other place.

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

}}} // namespace openage::renderer::opengl

#endif // if WITH_OPENGL
