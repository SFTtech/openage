// Copyright 2017-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <vulkan/vulkan.h>

#include "../renderer.h"

#include "graphics_device.h"


namespace openage {
namespace renderer {
namespace vulkan {

/// A renderer using the Vulkan API.
class VlkRenderer {
	VkInstance instance;

	VkSurfaceKHR surface;

public:
	VlkRenderer(VkInstance instance, VkSurfaceKHR surface)
		: instance(instance)
		, surface(surface) {}

	void do_the_thing();
};

}}} // openage::renderer::vulkan
