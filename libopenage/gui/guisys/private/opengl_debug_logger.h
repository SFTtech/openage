// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <QOpenGLFunctions>

QT_FORWARD_DECLARE_CLASS(QOpenGLContext)

namespace qtsdl {
struct gl_debug_parameters {
	/**
	 * True if the GL context is a debug context
	 */
	bool is_debug;

	/**
	 * Function that GL context uses to report debug messages
	 */
	GLvoid *callback;

	/**
	 * True if debug callback calling method is chosen to be synchronous
	 */
	bool synchronous;
};

/**
 * Get debugging settings of the current GL context
 *
 * @param current_source_context current GL context
 * @return debugging settings
 */
gl_debug_parameters get_current_opengl_debug_parameters(const QOpenGLContext &current_source_context);

/**
 * Create a GL logger in the current GL context
 *
 * @param params debugging settings
 * @param current_dest_context current GL context to which parameters will be applied
 */
void apply_opengl_debug_parameters(gl_debug_parameters params, QOpenGLContext &current_dest_context);

}
