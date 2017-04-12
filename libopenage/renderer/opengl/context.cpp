// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "context.h"

#include <epoxy/gl.h>

#include "../../log/log.h"
#include "../../error/error.h"


namespace openage {
namespace renderer {
namespace opengl {

/// The first element is the lowest version we need, last is highest version we support.
static constexpr std::array<std::pair<int, int>, 1> gl_versions = {{ { 3, 3 } }}; // for now we don't need any higher versions

/// Finds out the supported graphics functions and OpenGL version of the device.
static gl_context_capabilities find_capabilities() {
	// This is really hacky. We try to create a context starting with
	// the lowest GL version and retry until one version is not supported and fails.
	// There is no other way to do this. (https://gamedev.stackexchange.com/a/28457)

	SDL_Window *test_window = SDL_CreateWindow("test", 0, 0, 2, 2, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
	if (test_window == nullptr) {
		throw Error(MSG(err) << "Failed creating window for OpenGL context testing. SDL Error: " << SDL_GetError());
	}

	// Check each version for availability
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	SDL_GLContext test_context;
	for (size_t i_ver = 0; i_ver < gl_versions.size(); ++i_ver) {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_versions[i_ver].first);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_versions[i_ver].second);
		test_context = SDL_GL_CreateContext(test_window);

		if (test_context == nullptr) {
			if (i_ver == 0) {
				throw Error(MSG(err) << "OpenGL version "
				                     << gl_versions[0].first << "." << gl_versions[0].second
				                     << " is not available. It is the minimal required version.");
			}
			else {
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_versions[i_ver - 1].first);
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_versions[i_ver - 1].second);
				break;
			}
		}

		SDL_GL_DeleteContext(test_context);
	}

	test_context = SDL_GL_CreateContext(test_window);
	if (test_context == nullptr) {
		throw Error(MSG(err) << "Failed to create OpenGL context which previously succeeded. This should not happen! SDL Error: " << SDL_GetError());
	}
	SDL_GL_MakeCurrent(test_window, test_context);

	gl_context_capabilities caps;

	GLint temp;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &temp);
	caps.max_texture_size = temp;
	// TOOD maybe GL_MAX_TEXTURE_IMAGE_UNITS or maybe GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS
	// lol opengl
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &temp);
	caps.max_texture_slots = temp;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &temp);
	caps.max_vertex_attributes = temp;

	glGetIntegerv(GL_MAJOR_VERSION, &caps.major_version);
	glGetIntegerv(GL_MINOR_VERSION, &caps.minor_version);

	SDL_GL_DeleteContext(test_context);
	SDL_DestroyWindow(test_window);

	return caps;
}

GlContext::GlContext(SDL_Window *window) {
	this->capabilities = find_capabilities();
	auto const &capabilities = this->capabilities;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, capabilities.major_version);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, capabilities.minor_version);

	this->gl_context = SDL_GL_CreateContext(window);

	if (this->gl_context == nullptr) {
		throw Error(MSG(err) << "OpenGL context creation failed. SDL error: " << SDL_GetError());
	}

	// We still have to verify that our version of libepoxy supports this version of OpenGL.
	int epoxy_glv = capabilities.major_version * 10 + capabilities.minor_version;
	if (!epoxy_is_desktop_gl() || epoxy_gl_version() < epoxy_glv) {
		throw Error(MSG(err) << "The used version of libepoxy does not support OpenGL version "
		                     << capabilities.major_version << "." << capabilities.minor_version);
	}

	log::log(MSG(info) << "Created OpenGL context version " << capabilities.major_version << "." << capabilities.minor_version);

	// To quote the standard doc: 'The value gives a rough estimate of the
	// largest texture that the GL can handle'
	// -> wat?  anyways, we need at least 1024x1024.
	log::log(MSG(dbg) << "Maximum supported texture size: "
	                  << capabilities.max_texture_size);
	if (capabilities.max_texture_size < 1024) {
		throw Error(MSG(err) << "Maximum supported texture size is too small: "
		                     << capabilities.max_texture_size);
	}

	log::log(MSG(dbg) << "Maximum supported texture units: "
	                  << capabilities.max_texture_slots);
	if (capabilities.max_texture_slots < 2) {
		throw Error(MSG(err) << "Your GPU doesn't have enough texture units: "
		                     << capabilities.max_texture_slots);
	}
}

GlContext::~GlContext() {
	if (this->gl_context != nullptr) {
		SDL_GL_DeleteContext(this->gl_context);
	}
}

GlContext::GlContext(GlContext &&other)
	: gl_context(other.gl_context)
	, capabilities(other.capabilities) {
	other.gl_context = nullptr;
}

GlContext& GlContext::operator=(GlContext &&other) {
	this->gl_context = other.gl_context;
	this->capabilities = other.capabilities;
	other.gl_context = nullptr;

	return *this;
}

SDL_GLContext GlContext::get_raw_context() const {
	return this->gl_context;
}

gl_context_capabilities GlContext::get_capabilities() const {
	return this->capabilities;
}

void GlContext::check_error() {
	GLenum error_state = glGetError();
	if (error_state != GL_NO_ERROR) {
		const char *msg = [=] {
			// generate error message
			switch (error_state) {
			case GL_INVALID_ENUM:
				// An unacceptable value is specified for an enumerated argument.
				// The offending command is ignored
				// and has no other side effect than to set the error flag.
				return "invalid enum passed to opengl call";
			case GL_INVALID_VALUE:
				// A numeric argument is out of range.
				// The offending command is ignored
				// and has no other side effect than to set the error flag.
				return "invalid value passed to opengl call";
			case GL_INVALID_OPERATION:
				// The specified operation is not allowed in the current state.
				// The offending command is ignored
				// and has no other side effect than to set the error flag.
				return "invalid operation performed during some state";
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				// The framebuffer object is not complete. The offending command
				// is ignored and has no other side effect than to set the error flag.
				return "invalid framebuffer operation";
			case GL_OUT_OF_MEMORY:
				// There is not enough memory left to execute the command.
				// The state of the GL is undefined,
				// except for the state of the error flags,
				// after this error is recorded.
				return "out of memory, wtf?";
			case GL_STACK_UNDERFLOW:
				// An attempt has been made to perform an operation that would
				// cause an internal stack to underflow.
				return "stack underflow";
			case GL_STACK_OVERFLOW:
				// An attempt has been made to perform an operation that would
				// cause an internal stack to overflow.
				return "stack overflow";
			default:
				// unknown error state
				return "unknown error";
			}
		}();

		throw Error(
			MSG(err) << "An OpenGL error has occured.\n\t"
			<< "(" << error_state << "): " << msg
		);
	}
}

void GlContext::set_vsync(bool on) {
	if (on) {
		// try to use swap control tearing (adaptive vsync)
		if (SDL_GL_SetSwapInterval(-1) == -1) {
			// otherwise fall back to standard vsync
			SDL_GL_SetSwapInterval(1);
		}
	}
	else {
		SDL_GL_SetSwapInterval(0);
	}
}

/*
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
*/

}}} // openage::renderer::opengl
