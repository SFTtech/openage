// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#include "gl_debug.h"

#include <epoxy/gl.h>

#include "error.h"

namespace openage {
namespace error {

namespace {
void callback(GLenum source, GLenum, GLuint, GLenum, GLsizei, const GLchar *message, const void*) {
	const char *source_name;

	switch (source) {
	case GL_DEBUG_SOURCE_API:
		source_name = "API";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		source_name = "window system";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		source_name = "shader compiler";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		source_name = "third party";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		source_name = "application";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		source_name = "other";
		break;
	default:
		source_name = "unknown";
		break;
	}

	throw Error(MSG(err) << "OpenGL error from " << source_name << ": '" << message << "'.");
}

}

SDL_GLContext create_debug_context(SDL_Window *window) {
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

	auto ctx = SDL_GL_CreateContext(window);

	if (ctx != nullptr) {
		GLint flags;
		glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

		if (!(flags & GL_CONTEXT_FLAG_DEBUG_BIT))
			throw Error(MSG(err)<< "Failed creating a debug OpenGL context.");

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_FALSE);

		glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_ERROR, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, GL_DONT_CARE, 0, nullptr, GL_TRUE);

		glDebugMessageCallback(callback, nullptr);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}

	return ctx;
}

}} // openage::error
