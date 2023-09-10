// Copyright 2017-2023 the openage authors. See copying.md for legal info.

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

struct vlk_spec {
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

	VkInstance get_instance() const;
	VkSurfaceKHR get_surface() const;

private:
	vlk_spec capabilities;

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
