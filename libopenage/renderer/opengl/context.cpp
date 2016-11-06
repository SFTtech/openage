// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "../../config.h"
#if WITH_OPENGL

#include "context.h"

#include <epoxy/gl.h>
#include <SDL2/SDL.h>

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


std::shared_ptr<renderer::Texture> Context::register_texture(const TextureData &data) {
	std::shared_ptr<renderer::Texture> txt = std::make_shared<opengl::Texture>(data);
	return txt;
}

std::shared_ptr<renderer::Program> Context::register_program(const ProgramSource &data) {
	std::shared_ptr<renderer::Program> txt = std::make_shared<opengl::Program>(data);
	return txt;
}



}}} // namespace openage::renderer::opengl

#endif // if WITH_OPENGL
