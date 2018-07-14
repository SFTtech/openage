// Copyright 2017-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <set>
#include <cstring>
#include <optional>

#include <vulkan/vulkan.h>

#include "../window.h"

#include "loader.h"


namespace openage {
namespace renderer {
namespace vulkan {

struct vlk_capabilities {
	/// Names of available layers.
	std::set<std::string> layers;
	/// Names of available extensions.
	// TODO are these only available when the corresponding layer is active?
	std::set<std::string> extensions;
};

// TODO dirty hack to graft vk functionality onto window.
// needs better structure (not inheritance! (?)) for proper support
class VlkWindow : public openage::renderer::Window {
	vlk_capabilities capabilities;

	VkInstance instance;
	VkSurfaceKHR surface;
#ifndef NDEBUG
	VkDebugReportCallbackEXT debug_callback;
#endif
	VlkLoader loader;

	SDL_Window* window;

public:
	VlkWindow(const char* title);
	~VlkWindow();

	VkInstance get_instance() const;
	VkSurfaceKHR get_surface() const;
};

}}} // openage::renderer::vulkan
