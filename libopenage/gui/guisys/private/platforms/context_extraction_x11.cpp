// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include <cassert>

#include "context_extraction.h"

#include <QtPlatformHeaders/QGLXNativeContext>

#include "SDL2/SDL_syswm.h"
// It can't hurt to have this here even though OSX doesn't use X11 by default,
// you can use it with XQuartz
#ifndef __APPLE__
#include "GL/glx.h"
#else
#include "OpenGL/glx.h"
#endif

// DO NOT INCLUDE ANYTHING HERE, X11 HEADERS BREAK STUFF

namespace qtsdl {

std::tuple<QVariant, WId> extract_native_context(SDL_Window *window) {
	assert(window);

	GLXContext current_context;
	SDL_SysWMinfo wm_info;
	SDL_VERSION(&wm_info.version);
	if (SDL_GetWindowWMInfo(window, &wm_info)) {
		assert(wm_info.info.x11.display);

		current_context = glXGetCurrentContext();
		assert(current_context);
	}

	return std::make_tuple(QVariant::fromValue<QGLXNativeContext>(QGLXNativeContext(current_context, wm_info.info.x11.display, wm_info.info.x11.window)), wm_info.info.x11.window);
}

} // namespace qtsdl
