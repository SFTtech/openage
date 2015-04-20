// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_OPENGL_CONTEXT_H_
#define OPENAGE_RENDERER_OPENGL_CONTEXT_H_

#include <SDL2/SDL.h>

#include "../context.h"
#include "../../config.h"


namespace openage {
namespace renderer {

class GLContext : public Context {
public:
	GLContext();
	~GLContext();

	SDL_GLContext glcontext;

	virtual void prepare();
	virtual uint32_t get_window_flags();
	virtual void create(SDL_Window *window);
	virtual void setup();
	virtual void destroy();
};

}} // namespace openage::renderer

#endif
