// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "../../config.h"
#if WITH_OPENGL

#include "context.h"

#include <utility>
#include <array>

#include <epoxy/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "buffer.h"
#include "program.h"
#include "texture.h"
#include "vertex_state.h"
#include "../../log/log.h"
#include "../../error/error.h"

namespace openage {
namespace renderer {
namespace opengl {

Context::Context()
	:
	renderer::Context{context_type::opengl} {}

Context::~Context() {}

uint32_t Context::get_window_flags() const {
	return SDL_WINDOW_OPENGL;
}

// first element is the min supported version
// last element is the max supported version
constexpr std::array<std::pair<int, int>, 6> gl_versions = {{{3,3}, {4,0}, {4,1}, {4,2}, {4,3}, {4,4}}};

void Context::prepare() {
	// set initially to maximum supported version so that if the for loop doesn't fail the max one is used
	int opengl_version_major = gl_versions[gl_versions.size() - 1].first;
	int opengl_version_minor = gl_versions[gl_versions.size() - 1].second;

	SDL_Window *test_window = SDL_CreateWindow("test", 0, 0, 2, 2, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
	if (test_window == nullptr) {
		throw Error(MSG(err) << "Failed creating window for OpenGL context testing. SDL Error: " << SDL_GetError());
	}
	SDL_GLContext test_context;

	// check each version for availability
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	for (size_t i_ver = 0; i_ver < gl_versions.size(); ++i_ver) {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_versions[i_ver].first);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_versions[i_ver].second);
		test_context = SDL_GL_CreateContext(test_window);

		if (test_context == nullptr) {
			if (i_ver == 0) {
				throw Error(MSG(err) << "OpenGL version " << gl_versions[0].first << "." << gl_versions[0].second << " is not available. It is the minimal required version.");
			}
			else {
				opengl_version_major = gl_versions[i_ver - 1].first;
				opengl_version_minor = gl_versions[i_ver - 1].second;
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, opengl_version_major);
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, opengl_version_minor);
				break;
			}
		}

		SDL_GL_DeleteContext(test_context);
	}

	test_context = SDL_GL_CreateContext(test_window);
	if (test_context == nullptr) {
		throw Error(MSG(err) << "Failed to create OpenGL context which previously succeeded. This should not happen! SDL Error: " << SDL_GetError());
	}

	auto &cap = this->capability;

	cap.type = this->type;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &cap.max_texture_size);
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &cap.max_texture_slots);
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &cap.max_vertex_attributes);

	glGetIntegerv(GL_MAJOR_VERSION, &cap.major_version);
	glGetIntegerv(GL_MINOR_VERSION, &cap.minor_version);

	SDL_GL_DeleteContext(test_context);
	SDL_DestroyWindow(test_window);
}

void Context::create(SDL_Window *window) {
	this->glcontext = SDL_GL_CreateContext(window);

	if (this->glcontext == nullptr) {
		throw Error(MSG(err) << "Failed creating OpenGL context: " << SDL_GetError());
	}

	// check the OpenGL version, for shaders n stuff
	// TODO: this doesn't look necessary. libepoxy supports up to 4.4, which is the maximum supported version in gl_versions
	// and if GL 3.3 is not available than the context above will fail anyway and it won't get to this point
	int epoxy_glv =  this->capability.major_version * 10 + this->capability.minor_version;
	if (not epoxy_is_desktop_gl() or epoxy_gl_version() < epoxy_glv) {
		throw Error(MSG(err) << "OpenGL "
		                     << this->capability.major_version << "." << this->capability.minor_version
		                     << " not available");
	}

	log::log(MSG(info) << "Using OpenGL " << this->capability.major_version << "." << this->capability.minor_version);
}

void Context::setup() {
	// TODO: context capability checking
	auto &caps = this->capability;

	// to quote the standard doc: 'The value gives a rough estimate of the
	// largest texture that the GL can handle'
	// -> wat?  anyways, we need at least 1024x1024.
	log::log(MSG(dbg) << "Maximum supported texture size: "
	         << caps.max_texture_size);
	if (caps.max_texture_size < 1024) {
		throw Error(MSG(err) << "Maximum supported texture size too small: "
		            << caps.max_texture_size);
	}

	log::log(MSG(dbg) << "Maximum supported texture units: "
	         << caps.max_texture_slots);
	if (caps.max_texture_slots < 2) {
		throw Error(MSG(err) << "Your GPU has not enough texture units: "
		            << caps.max_texture_slots);
	}


	// vsync on
	// TODO: maybe move somewhere else or to the window.
	SDL_GL_SetSwapInterval(1);

	// TODO: move to somewhere else, not all contexts may want those:

	// enable alpha blending
	this->set_feature(context_feature::blending, true);

	// order of drawing relevant for depth
	// what gets drawn last is displayed on top.
	this->set_feature(context_feature::depth_test, false);

	// TODO: generalize like set_feature.
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


context_capability Context::get_capabilities() {
	return this->capability;
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


void Context::check_error() {
	int glerrorstate = 0;

	glerrorstate = glGetError();
	if (glerrorstate != GL_NO_ERROR) {

		const char *errormsg;

		//generate error message
		switch (glerrorstate) {
		case GL_INVALID_ENUM:
			// An unacceptable value is specified for an enumerated argument.
			// The offending command is ignored
			// and has no other side effect than to set the error flag.
			errormsg = "invalid enum passed to opengl call";
			break;
		case GL_INVALID_VALUE:
			// A numeric argument is out of range.
			// The offending command is ignored
			// and has no other side effect than to set the error flag.
			errormsg = "invalid value passed to opengl call";
			break;
		case GL_INVALID_OPERATION:
			// The specified operation is not allowed in the current state.
			// The offending command is ignored
			// and has no other side effect than to set the error flag.
			errormsg = "invalid operation performed during some state";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			// The framebuffer object is not complete. The offending command
			// is ignored and has no other side effect than to set the error flag.
			errormsg = "invalid framebuffer operation";
			break;
		case GL_OUT_OF_MEMORY:
			// There is not enough memory left to execute the command.
			// The state of the GL is undefined,
			// except for the state of the error flags,
			// after this error is recorded.
			errormsg = "out of memory, wtf?";
			break;
		case GL_STACK_UNDERFLOW:
			// An attempt has been made to perform an operation that would
			// cause an internal stack to underflow.
			errormsg = "stack underflow";
			break;
		case GL_STACK_OVERFLOW:
			// An attempt has been made to perform an operation that would
			// cause an internal stack to overflow.
			errormsg = "stack overflow";
			break;
		default:
			// unknown error state
			errormsg = "unknown error";
		}
		throw Error(
			MSG(err) <<
			"OpenGL error state id: " << glerrorstate
			<< "\n\t" << errormsg
		);
	}
}


std::unique_ptr<renderer::Texture> Context::register_texture(const TextureData &data) {
	std::unique_ptr<renderer::Texture> txt = std::make_unique<opengl::Texture>(this, data);
	return txt;
}

std::unique_ptr<renderer::Program> Context::register_program(const ProgramSource &data) {
	std::unique_ptr<renderer::Program> prg = std::make_unique<opengl::Program>(this, data);
	return prg;
}

std::unique_ptr<renderer::Buffer> Context::create_buffer(size_t size) {
	std::unique_ptr<renderer::Buffer> buf = std::make_unique<opengl::Buffer>(this, size);
	return buf;
}

std::unique_ptr<renderer::VertexState> Context::create_vertex_state() {
	std::unique_ptr<renderer::VertexState> vstate = std::make_unique<opengl::VertexState>(this);
	return vstate;
}



void Context::resize_canvas(const coord::window &new_size) {
	log::log(MSG(dbg) << "opengl viewport resize to " << new_size.x << "x" << new_size.y);

	glViewport(0, 0, new_size.x, new_size.y);
}


}}} // namespace openage::renderer::opengl

#endif // if WITH_OPENGL
