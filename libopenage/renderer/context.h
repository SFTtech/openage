// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_CONTEXT_H_
#define OPENAGE_RENDERER_CONTEXT_H_

#include <SDL2/SDL.h>

#include <memory>

#include "../coord/window.h"


namespace openage {
namespace renderer {

enum class context_type {
	autodetect,
	opengl,
	vulkan,
};

class Context {
public:
	Context();
	virtual ~Context();

	static std::unique_ptr<Context> generate(context_type t);

	virtual void prepare() = 0;
	virtual uint32_t get_window_flags() = 0;
	virtual void create(SDL_Window *window) = 0;
	virtual void setup() = 0;
	virtual void destroy() = 0;
};

}} // namespace openage::renderer

#endif
