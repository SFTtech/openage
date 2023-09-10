// Copyright 2017-2023 the openage authors. See copying.md for legal info.

#include "windowvk.h"

#include <set>

#include <QGuiApplication>
#include <QQuickWindow>
#include <QVulkanInstance>

#include "error/error.h"
#include "log/log.h"
#include "renderer/window_event_handler.h"

#include "renderer/vulkan/graphics_device.h"
#include "renderer/vulkan/util.h"


namespace openage::renderer::vulkan {

#ifndef NDEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL vlk_debug_cb(
	VkDebugReportFlagsEXT /*flags*/,
	VkDebugReportObjectTypeEXT /*objType*/,
	uint64_t /*obj*/,
	size_t /*location*/,
	int32_t /*code*/,
	const char *layerPrefix,
	const char *msg,
	void * /*userData*/) {
	log::log(MSG(dbg) << layerPrefix << " " << msg);

	return VK_FALSE;
}
#endif

/// Queries the Vulkan implementation for available extensions and layers.
static vlk_spec find_spec() {
	vlk_spec specs;

	// Find which layers are available.
	auto layers = vk_do_ritual(vkEnumerateInstanceLayerProperties);

	log::log(MSG(dbg) << "Available Vulkan layers:");
	for (auto const &lr : layers) {
		specs.layers.insert(lr.layerName);
		log::log(MSG(dbg) << "\t" << lr.layerName);
	}

	// Find which extensions are available.
	// This is annoying, since an enumeration call without a filter-by-layer parameter
	// won't return all extensions. We thus have to enumerate extensions for each layer
	// and then remove duplicates.

	// First retrieve non-layer extensions.
	auto props = vk_do_ritual(vkEnumerateInstanceExtensionProperties, nullptr);

	for (auto const &p : props) {
		specs.extensions.emplace(p.extensionName);
	}

	// Then retrieve extensions from layers.
	for (auto const &lr : layers) {
		auto lr_props = vk_do_ritual(vkEnumerateInstanceExtensionProperties, lr.layerName);

		for (auto const &p : lr_props) {
			specs.extensions.emplace(p.extensionName);
		}
	}

	log::log(MSG(dbg) << "Available Vulkan extensions:");
	for (const auto &ext : specs.extensions) {
		log::log(MSG(dbg) << "\t" << ext);
	}

	return specs;
}

VlkWindow::VlkWindow(const char *title, size_t width, size_t height) :
	Window(width, height), capabilities(find_spec()) {
	std::vector<const char *> extension_names;

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
	std::vector<const char *> layer_names;
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
	VkApplicationInfo app_info = {};
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

	if (QGuiApplication::instance() == nullptr) {
		// Qt windows need to attach to a QtGuiApplication
		throw Error{MSG(err) << "Failed to create Qt window: QGuiApplication has not been created yet."};
	}

	QQuickWindow::setGraphicsApi(QSGRendererInterface::Vulkan);
	this->window = std::make_shared<QWindow>();
	this->window->setTitle(QString::fromStdString(title));
	this->window->resize(width, height);
	this->window->setSurfaceType(QSurface::VulkanSurface);

	// Attach vulkan instance
	QVulkanInstance qinstance;
	qinstance.setVkInstance(this->instance);
	qinstance.create();
	QVulkanInstance::surfaceForWindow(this->window.get());
	this->window->setVulkanInstance(&qinstance);

	QSurfaceFormat format;
	format.setDepthBufferSize(16);
	format.setStencilBufferSize(8);
	this->window->setFormat(format);

	this->window->installEventFilter(this->event_handler.get());

	this->window->show();
	log::log(MSG(info) << "Created Qt window with Vulkan instance.");
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

} // namespace openage::renderer::vulkan
