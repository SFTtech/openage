// Copyright 2017-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <cstring>
#include <optional>
#include <set>

#include <vulkan/vulkan.h>

#include "../window.h"

#include "loader.h"

QT_FORWARD_DECLARE_CLASS(QWindow)

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
public:
	VlkWindow(const char *title, size_t width, size_t height);
	~VlkWindow();

	std::shared_ptr<SDL_Window> get_sdl_window() override;
	std::shared_ptr<QWindow> get_qt_window() override;

	VkInstance get_instance() const;
	VkSurfaceKHR get_surface() const;

private:
	SDL_Window *window;
	std::shared_ptr<QWindow> qwindow;

	vlk_capabilities capabilities;

	VkInstance instance;
	VkSurfaceKHR surface;
#ifndef NDEBUG
	VkDebugReportCallbackEXT debug_callback;
#endif
	VlkLoader loader;
};

} // namespace vulkan
} // namespace renderer
} // namespace openage
