// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "opengl_debug_logger.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions_4_4_Core>

namespace qtsdl {

gl_debug_parameters get_current_opengl_debug_parameters(const QOpenGLContext &current_source_context)  {
	gl_debug_parameters params{};

	if (current_source_context.versionFunctions<QOpenGLFunctions_4_4_Core>())
		if ((params.is_debug = current_source_context.format().options().testFlag(QSurfaceFormat::DebugContext))) {
			glGetPointerv(GL_DEBUG_CALLBACK_FUNCTION, &params.callback);
			params.synchronous = glIsEnabled(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		}

	return params;
}

void apply_opengl_debug_parameters(gl_debug_parameters params, QOpenGLContext &current_dest_context)  {
	if (params.is_debug && params.callback) {
		if (auto functions = current_dest_context.versionFunctions<QOpenGLFunctions_4_4_Core>()) {
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

} // namespace qtsdl
