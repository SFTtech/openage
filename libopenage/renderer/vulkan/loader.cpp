#include "loader.h"

#include "../../error/error.h"


namespace openage {
namespace renderer {
namespace vulkan {

VlkLoader::VlkLoader()
	: inited(false) {}

void VlkLoader::init(VkInstance instance) {
	#ifndef NDEBUG
	this->pCreateDebugReportCallbackEXT = PFN_vkCreateDebugReportCallbackEXT(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
	this->pDestroyDebugReportCallbackEXT = PFN_vkDestroyDebugReportCallbackEXT(vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT"));
	#endif

	this->inited = true;
}

#ifndef NDEBUG
VkResult VlkLoader::vkCreateDebugReportCallbackEXT(
	VkInstance instance,
	const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugReportCallbackEXT* pCallback
) {
	if (!this->inited) {
		throw Error(MSG(err) << "Tried to request function from Vulkan extension loader before initializing it.");
	}

	if (this->pCreateDebugReportCallbackEXT != nullptr) {
		return this->pCreateDebugReportCallbackEXT(instance, pCreateInfo, pAllocator, pCallback);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void VlkLoader::vkDestroyDebugReportCallbackEXT(
	VkInstance instance,
	VkDebugReportCallbackEXT callback,
	const VkAllocationCallbacks* pAllocator
) {
	if (!this->inited) {
		throw Error(MSG(err) << "Tried to request function from Vulkan extension loader before initializing it.");
	}

	if (this->pDestroyDebugReportCallbackEXT != nullptr) {
		this->pDestroyDebugReportCallbackEXT(instance, callback, pAllocator);
	}
}
#endif

}}} // openage::renderer::vulkan
