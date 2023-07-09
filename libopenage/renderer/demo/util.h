// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once


namespace openage::renderer::tests {

// Macro for debugging OpenGL state.
#define DBG_GL(txt) \
	printf("before %s\n", txt); \
	opengl::GlContext::check_error(); \
	printf("after %s\n", txt);

} // namespace openage::renderer::tests
