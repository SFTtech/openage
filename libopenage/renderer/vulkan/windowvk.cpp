#include "windowvk.h"

#include <set>

#include <SDL2/SDL_vulkan.h>

#include "../../error/error.h"
#include "../../log/log.h"
#include "graphics_device.h"
#include "util.h"



namespace openage {
namespace renderer {
namespace vulkan {

#ifndef NDEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL vlk_debug_cb(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t obj,
	size_t location,
	int32_t code,
	const char* layerPrefix,
	const char* msg,
	void* userData)
{
	log::log(MSG(dbg) << layerPrefix << " " << msg);

	return VK_FALSE;
}
#endif

/// Queries the Vulkan implementation for available extensions and layers.
static vlk_capabilities find_capabilities() {
	vlk_capabilities caps;

	// Find which layers are available.
	auto layers = vk_do_ritual(vkEnumerateInstanceLayerProperties);

	log::log(MSG(dbg) << "Available Vulkan layers:");
	for (auto const& lr : layers) {
		caps.layers.insert(lr.layerName);
		log::log(MSG(dbg) << "\t" << lr.layerName);
	}

	// Find which extensions are available.
	// This is annoying, since an enumeration call without a filter-by-layer parameter
	// won't return all extensions. We thus have to enumerate extensions for each layer
	// and then remove duplicates.

	// First retrieve non-layer extensions.
	auto props = vk_do_ritual(vkEnumerateInstanceExtensionProperties, nullptr);

	for (auto const& p : props) {
		caps.extensions.emplace(p.extensionName);
	}

	// Then retrieve extensions from layers.
	for (auto const& lr : layers) {
		auto lr_props = vk_do_ritual(vkEnumerateInstanceExtensionProperties, lr.layerName);

		for (auto const& p : lr_props) {
			caps.extensions.emplace(p.extensionName);
		}
	}

	log::log(MSG(dbg) << "Available Vulkan extensions:");
	for (const auto& ext : caps.extensions) {
		log::log(MSG(dbg) << "\t" << ext);
	}

	return caps;
}

VlkWindow::VlkWindow(const char* title)
	: capabilities(find_capabilities())
{
	// Find which extensions the SDL window requires.
	auto extension_names = vk_do_ritual(SDL_Vulkan_GetInstanceExtensions, this->window);
	/*
	if (succ != SDL_TRUE) {
		throw Error(MSG(err) << "Failed to obtain required Vulkan extension names from SDL.");
	}
	*/

#ifndef NDEBUG
	extension_names.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif
	extension_names.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

	// Check if all extensions are available.
	for (auto ext : extension_names) {
		if (this->capabilities.extensions.count(ext) == 0) {
			throw Error(MSG(err) << "Vulkan driver is missing required extension: " << ext);
		}
	}

	// The names of Vulkan layers which we need.
	std::vector<const char*> layer_names;
#ifndef NDEBUG
	layer_names.push_back("VK_LAYER_LUNARG_standard_validation");
#endif

	// Check if all layers are available
	for (auto lr : layer_names) {
		if (this->capabilities.layers.count(lr) == 0) {
			throw Error(MSG(err) << "Vulkan driver is missing required layer: " << lr);
		}
	}

	// Setup application description.
	VkApplicationInfo app_info  = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = title;
	app_info.apiVersion = VK_MAKE_VERSION(1, 0, 57);

	VkInstanceCreateInfo inst_info = {};
	inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	inst_info.pApplicationInfo = &app_info;
	inst_info.enabledExtensionCount = extension_names.size();
	inst_info.ppEnabledExtensionNames = extension_names.data();
	inst_info.enabledLayerCount = layer_names.size();
	inst_info.ppEnabledLayerNames = layer_names.data();

	// A Vulkan instance is a proxy for all usage of Vulkan from our application,
	// kind of like a GL context. Initialize it.
	VK_CALL_CHECKED(vkCreateInstance, &inst_info, nullptr, &this->instance);

	this->loader.init(this->instance);

#ifndef NDEBUG
	VkDebugReportCallbackCreateInfoEXT cb_info = {};
	cb_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	cb_info.flags =
	VK_DEBUG_REPORT_ERROR_BIT_EXT
	| VK_DEBUG_REPORT_WARNING_BIT_EXT
	| VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT
	| VK_DEBUG_REPORT_INFORMATION_BIT_EXT
	| VK_DEBUG_REPORT_DEBUG_BIT_EXT;
	cb_info.pfnCallback = vlk_debug_cb;

	VK_CALL_CHECKED(this->loader.vkCreateDebugReportCallbackEXT, this->instance, &cb_info, nullptr, &this->debug_callback);
#endif

	// Surface is an object that we draw into, corresponding to the window area.
	auto succ = SDL_Vulkan_CreateSurface(this->window, this->instance, &this->surface);
	if (succ != SDL_TRUE) {
		throw Error(MSG(err) << "Failed to create Vulkan surface on SDL window.");
	}
}

VlkWindow::~VlkWindow() {
#ifndef NDEBUG
	this->loader.vkDestroyDebugReportCallbackEXT(this->instance, this->debug_callback, nullptr);
#endif
	vkDestroySurfaceKHR(this->instance, this->surface, nullptr);
	vkDestroyInstance(this->instance, nullptr);
}

VkInstance VlkWindow::get_instance() const {
	return this->instance;
}

VkSurfaceKHR VlkWindow::get_surface() const {
	return this->surface;
}

}}} // openage::renderer::vulkan
