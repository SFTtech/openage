#pragma once

#include <vulkan/vulkan.h>


namespace openage {
namespace renderer {
namespace vulkan {

/// A class for dynamically loading Vulkan extension functions.
class VlkLoader {
#ifndef NDEBUG
	PFN_vkCreateDebugReportCallbackEXT pCreateDebugReportCallbackEXT;
	PFN_vkDestroyDebugReportCallbackEXT pDestroyDebugReportCallbackEXT;
#endif

	bool inited;

public:
	VlkLoader();

	/// Initialize this loader for the given Vulkan instance.
	void init(VkInstance);

#ifndef NDEBUG
	/// Part of VK_EXT_debug_report, allows setting a callback for debug events.
	VkResult vkCreateDebugReportCallbackEXT(
		VkInstance instance,
		const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugReportCallbackEXT* pCallback
	);


	/// Part of VK_EXT_debug_report, destroys the debug callback object.
	void vkDestroyDebugReportCallbackEXT(
		VkInstance instance,
		VkDebugReportCallbackEXT callback,
		const VkAllocationCallbacks* pAllocator
	);
#endif
};

}}} // openage::renderer::vulkan
