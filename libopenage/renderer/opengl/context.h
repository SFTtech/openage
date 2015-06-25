// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_OPENGL_CONTEXT_H_
#define OPENAGE_RENDERER_OPENGL_CONTEXT_H_

#include <SDL2/SDL.h>
#include <memory>

#include "../context.h"


namespace openage {
namespace renderer {
namespace opengl {

class Context : public renderer::Context {
public:
	Context();
	~Context();

	SDL_GLContext glcontext;

	virtual void prepare();
	virtual uint32_t get_window_flags();
	virtual void create(SDL_Window *window);
	virtual void setup();
	virtual void destroy();
};

}}} // namespace openage::renderer

#endif
