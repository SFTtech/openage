// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include <cassert>

#include "context_extraction.h"

#include <QtPlatformHeaders/QCocoaNativeContext>

#include "SDL_syswm.h"
#import <AppKit/AppKit.h>

namespace qtsdl {

std::tuple<QVariant, WId> extract_native_context(SDL_Window *window) {
	assert(window);

	NSOpenGLContext *current_context = [NSOpenGLContext currentContext];
	assert(current_context);

	NSView *view = nullptr;

	SDL_SysWMinfo wm_info;
	SDL_VERSION(&wm_info.version);

	if (SDL_GetWindowWMInfo(window, &wm_info)) {
		NSWindow *ns_window = wm_info.info.cocoa.window;
		view = [ns_window contentView];
	}

	assert(view);

	return std::make_tuple(QVariant::fromValue<QCocoaNativeContext>(QCocoaNativeContext(current_context)), reinterpret_cast<WId>(view));
}

} // namespace qtsdl
