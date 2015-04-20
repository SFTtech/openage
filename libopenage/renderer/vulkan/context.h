// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_VULKAN_CONTEXT_H_
#define OPENAGE_RENDERER_VULKAN_CONTEXT_H_

#include <SDL2/SDL.h>

#include "../context.h"
#include "../../config.h"


namespace openage {
namespace renderer {

class VulkanContext : public Context {
public:
	SDL_VulkanContext vkcontext;

	virtual void prepare() = 0;
	virtual uint32_t get_window_flags() = 0;
	virtual void create(SDL_Window *window) = 0;
	virtual void setup() = 0;
	virtual void destroy() = 0;
};

}} // namespace openage::renderer

#endif
