// Copyright 2017-2024 the openage authors. See copying.md for legal info.

#include "opengl_debug_logger.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLVersionFunctionsFactory>

#ifdef __APPLE__
	// from https://www.khronos.org/registry/OpenGL/api/GL/glext.h
	#define GL_DEBUG_CALLBACK_FUNCTION 0x8244
	#define GL_DEBUG_OUTPUT_SYNCHRONOUS 0x8242
	#define GL_DEBUG_TYPE_ERROR 0x824C
	#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR 0x824E
#endif

namespace qtgui {

gl_debug_parameters get_current_opengl_debug_parameters(QOpenGLContext &current_source_context) {
	gl_debug_parameters params{};

	if (QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_4_4_Core>(&current_source_context))
		if ((params.is_debug = current_source_context.format().options().testFlag(QSurfaceFormat::DebugContext))) {
			glGetPointerv(GL_DEBUG_CALLBACK_FUNCTION, &params.callback);
			params.synchronous = glIsEnabled(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		}

	return params;
}

void apply_opengl_debug_parameters(gl_debug_parameters params, QOpenGLContext &current_dest_context) {
	if (params.is_debug && params.callback) {
		if (auto functions = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_4_4_Core>(&current_dest_context)) {
			functions->initializeOpenGLFunctions();

			functions->glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_FALSE);

			functions->glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_ERROR, GL_DONT_CARE, 0, nullptr, GL_TRUE);
			functions->glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, GL_DONT_CARE, 0, nullptr, GL_TRUE);

			functions->glDebugMessageCallback((GLDEBUGPROC)params.callback, nullptr);

			if (params.synchronous)
				glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		}
	}
}

} // namespace qtgui
