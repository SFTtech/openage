// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include <cassert>

#include <windows.h>
#include "context_extraction.h"

#include <QtPlatformHeaders/QWGLNativeContext>
#include <SDL2/SDL_syswm.h>
#include <GL/gl.h>

namespace qtsdl {

std::tuple<QVariant, WId> extract_native_context(SDL_Window *window) {
	assert(window);

	HGLRC current_context;
	SDL_SysWMinfo wm_info;
	SDL_VERSION(&wm_info.version);
	if (SDL_GetWindowWMInfo(window, &wm_info)) {
		current_context = wglGetCurrentContext();
		assert(current_context);
	}
	QWGLNativeContext nativeContext(current_context, wm_info.info.win.window);
	return {QVariant::fromValue(nativeContext), reinterpret_cast<WId>(wm_info.info.win.window)};
}

std::tuple<QVariant, std::function<void()>> extract_native_context_and_switchback_func(SDL_Window *window) {
	assert(window);

	HGLRC current_context;
	SDL_SysWMinfo wm_info;
	SDL_VERSION(&wm_info.version);
	if (SDL_GetWindowWMInfo(window, &wm_info)) {
		current_context = wglGetCurrentContext();
		assert(current_context);

		return std::make_tuple(QVariant::fromValue<QWGLNativeContext>(QWGLNativeContext(current_context, wm_info.info.win.window)), [wm_info, current_context] {
			wglMakeCurrent(wm_info.info.win.hdc, current_context);
		});
	}

	return std::tuple<QVariant, std::function<void()>>{};
}


} // namespace qtsdl
