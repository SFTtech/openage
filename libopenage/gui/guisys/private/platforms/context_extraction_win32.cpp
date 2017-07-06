// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include <cassert>

#include "windows.h"
#include "context_extraction.h"

#include <QtPlatformHeaders/QWGLNativeContext>

#include "SDL2/SDL_syswm.h"
#include "GL/gl.h"

namespace qtsdl {

std::tuple<QVariant, WId> extract_native_context(SDL_Window *window) {
	assert(window);

	HGLRC current_context;
	SDL_SysWMinfo wm_info;
	SDL_VERSION(&wm_info.version);
	if (SDL_GetWindowWMInfo(window, &wm_info)) {
#ifndef NDEBUGwglGetCurrentContext
#endif
		current_context = wglGetCurrentContext();
		assert(current_context);
	}
    QWGLNativeContext nativeContext(current_context, wm_info.info.win.window);
	return std::make_tuple(QVariant::fromValue(nativeContext), (WId)wm_info.info.win.window);
}

} // namespace qtsdl
