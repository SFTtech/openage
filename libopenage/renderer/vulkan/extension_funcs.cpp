#include <set>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>


/// This whole file is a _HORRIBLE HACK_ to get extensions functions to work easily.
/// It lazily creates a VkInstance with all possible extensions and gets the function
/// addresses from that. Missing extension functions are then defined here with a dispatch
/// to the proper address. This might fail, crash and burn if you use an extension function
/// with an instance that doesn't support it or if two extensions provide the same function.

static bool inited = false;
static PFN_vkCreateDebugReportCallbackEXT pCreateDebugReportCallbackEXT;
static PFN_vkDestroyDebugReportCallbackEXT pDestroyDebugReportCallbackEXT;

static void make_ext_available() {
	if (!inited) {
		uint32_t count = 0;
		vkEnumerateInstanceLayerProperties(&count, nullptr);
		std::vector<VkLayerProperties> layers(count);
		vkEnumerateInstanceLayerProperties(&count, layers.data());

		std::set<std::string> layer_names;
		for (auto const& lr : layers) {
			layer_names.insert(lr.layerName);
		}

		std::vector<VkExtensionProperties> props(32);

		vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
		if (props.size() < count) { props.resize(count); }
		vkEnumerateInstanceExtensionProperties(nullptr, &count, props.data());

		std::set<std::string> extension_names;
		for (size_t i = 0; i < count; i++) {
			extension_names.emplace(props[i].extensionName);
		}

		for (auto const& lr : layers) {
			vkEnumerateInstanceExtensionProperties(lr.layerName, &count, nullptr);
			if (props.size() < count) { props.resize(count); }
			vkEnumerateInstanceExtensionProperties(lr.layerName, &count, props.data());

			for (size_t i = 0; i < count; i++) {
				extension_names.emplace(props[i].extensionName);
			}
		}

		VkApplicationInfo app_info {};
		app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		app_info.pApplicationName = "HACKHACKHACK";
		app_info.apiVersion = VK_MAKE_VERSION(1, 0, 57);

		std::vector<const char*> c_layer_names;
		for (auto const& lr : layer_names) {
			c_layer_names.push_back(lr.c_str());
		}
		std::vector<const char*> c_ext_names;
		for (auto const& ext : extension_names) {
			c_ext_names.push_back(ext.c_str());
		}
		VkInstanceCreateInfo inst_info {};
		inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		inst_info.pApplicationInfo = &app_info;
		inst_info.enabledExtensionCount = c_ext_names.size();
		inst_info.ppEnabledExtensionNames = c_ext_names.data();
		inst_info.enabledLayerCount = c_layer_names.size();
		inst_info.ppEnabledLayerNames = c_layer_names.data();

		VkInstance instance;
		VkResult res = vkCreateInstance(&inst_info, nullptr, &instance);
		if (res != VK_SUCCESS) {
			throw std::string("Failed to HACKHACKHACK.");
		}

		pCreateDebugReportCallbackEXT = PFN_vkCreateDebugReportCallbackEXT(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
		pDestroyDebugReportCallbackEXT = PFN_vkDestroyDebugReportCallbackEXT(vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT"));

		inited = true;
	}
}

/// -- VK_EXT_debug_report --
VkResult vkCreateDebugReportCallbackEXT(
	VkInstance instance,
	const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugReportCallbackEXT* pCallback) {
	make_ext_available();

	if (pCreateDebugReportCallbackEXT != nullptr) {
		return pCreateDebugReportCallbackEXT(instance, pCreateInfo, pAllocator, pCallback);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void vkDestroyDebugReportCallbackEXT(
	VkInstance instance,
	VkDebugReportCallbackEXT callback,
	const VkAllocationCallbacks* pAllocator
) {
	make_ext_available();

	if (pDestroyDebugReportCallbackEXT != nullptr) {
		pDestroyDebugReportCallbackEXT(instance, callback, pAllocator);
	}
}
