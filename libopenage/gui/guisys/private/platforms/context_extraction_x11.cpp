// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include <cassert>

#include "context_extraction.h"

#include <QtPlatformHeaders/QGLXNativeContext>
#include <SDL2/SDL_syswm.h>
#include <GL/glx.h>

#include "../../../../error/error.h"

// DO NOT INCLUDE ANYTHING HERE, X11 HEADERS BREAK STUFF

namespace qtsdl {

std::tuple<QVariant, WId> extract_native_context(SDL_Window *window) {
	assert(window);

	GLXContext current_context;
	SDL_SysWMinfo wm_info;
	SDL_VERSION(&wm_info.version);

	if (!SDL_GetWindowWMInfo(window, &wm_info)) {
		throw openage::Error{ERR << "SDL: Could not get window manager info: " << SDL_GetError()};
	}

	assert(wm_info.info.x11.display);

	current_context = glXGetCurrentContext();
	assert(current_context);

	return std::make_tuple(
		QVariant::fromValue<QGLXNativeContext>(
			QGLXNativeContext(current_context,
			                  wm_info.info.x11.display,
			                  wm_info.info.x11.window)),
		wm_info.info.x11.window
	);
}

} // namespace qtsdl
