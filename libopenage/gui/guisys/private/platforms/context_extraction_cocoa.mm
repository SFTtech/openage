// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include <cassert>

#include "context_extraction.h"

#include <QtPlatformHeaders/QCocoaNativeContext>

#include "SDL_syswm.h"
#import <AppKit/AppKit.h>

namespace qtsdl {

std::tuple<QVariant, WId> extract_native_context(SDL_Window *window) {
	return std::tuple<QVariant, WId>{};
}

std::tuple<QVariant, std::function<void()>> extract_native_context_and_switchback_func(SDL_Window *window) {
	assert(window);

	NSOpenGLContext *current_context = [NSOpenGLContext currentContext];
	assert(current_context);

	return std::make_tuple(QVariant::fromValue<QCocoaNativeContext>(QCocoaNativeContext(current_context)), std::bind(SDL_GL_MakeCurrent, window, SDL_GL_GetCurrentContext()));
}

} // namespace qtsdl
