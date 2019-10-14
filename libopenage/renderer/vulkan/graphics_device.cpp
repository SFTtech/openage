// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#include "graphics_device.h"

#include <cstring>

#include "../../error/error.h"
#include "../../log/log.h"

#include "util.h"


namespace openage::renderer::vulkan {

std::optional<SurfaceSupportDetails> VlkGraphicsDevice::find_device_surface_support(VkPhysicalDevice dev, VkSurfaceKHR surf) {
	// Search for queue families in the device
	auto q_fams = vk_do_ritual(vkGetPhysicalDeviceQueueFamilyProperties, dev);

	std::optional<uint32_t> maybe_graphics_fam = {};
	std::optional<uint32_t> maybe_present_fam = {};

	// Figure out if any of the families supports graphics
	for (size_t i = 0; i < q_fams.size(); i++) {
		auto const& q_fam = q_fams[i];

		if (q_fam.queueCount > 0) {
			if ((q_fam.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0u) {
				maybe_graphics_fam = i;

				// See if it also supports present
				VkBool32 support = VK_FALSE;
				vkGetPhysicalDeviceSurfaceSupportKHR(dev, i, surf, &support);
				if (support != VK_FALSE) {
					// This family support both, we're done
					maybe_present_fam = i;
					break;
				}
			}
		}
	}

	if (!maybe_graphics_fam) {
		// This device has no graphics queue family that works with the surface
		return {};
	}

	SurfaceSupportDetails details = {};
	details.phys_device = dev;
	details.surface = surf;

	// If we have found a family that support both graphics and present
	if (maybe_present_fam) {
		details.graphics_fam = *maybe_graphics_fam;
		details.maybe_present_fam = {};
	} else {
		// Otherwise look for a present-only queue
		for (size_t i = 0; i < q_fams.size(); i++) {
			auto const& q_fam = q_fams[i];
			if (q_fam.queueCount > 0) {
				VkBool32 support = VK_FALSE;
				vkGetPhysicalDeviceSurfaceSupportKHR(dev, i, surf, &support);
				if (support != VK_FALSE) {
					maybe_present_fam = i;
					break;
				}
			}
		}

		if (!maybe_present_fam) {
			// This device has no present queue family that works with the surface
			return {};
		}

		details.graphics_fam = *maybe_graphics_fam;
		details.maybe_present_fam = maybe_present_fam;
	}

	// Obtain other information
	details.surface_formats = vk_do_ritual(vkGetPhysicalDeviceSurfaceFormatsKHR, dev, surf);
	details.present_modes = vk_do_ritual(vkGetPhysicalDeviceSurfacePresentModesKHR, dev, surf);
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(dev, surf, &details.surface_caps);

	// Finally, check that we have at least one format and present mode
	if (details.surface_formats.empty() || details.present_modes.empty()) {
		return {};
	}

	return details;
}

VlkGraphicsDevice::VlkGraphicsDevice(VkPhysicalDevice dev, std::vector<uint32_t> const& q_fams)
	: phys_device(dev)
{
	// Prepare queue creation info for each family requested
	std::vector<VkDeviceQueueCreateInfo> q_infos(q_fams.size());
	const float p = 1.0f;

	for (size_t i = 0; i < q_fams.size(); i++) {
		q_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		q_infos[i].queueFamilyIndex = q_fams[i];
		q_infos[i].queueCount = 1;
		q_infos[i].pQueuePriorities = &p;
	}

	// Request these extensions
	std::vector<const char*> ext_names = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	// Check if extensions are available
	auto exts = vk_do_ritual(vkEnumerateDeviceExtensionProperties, dev, nullptr);
	for (auto ext : ext_names) {
		if (std::count_if(exts.begin(), exts.end(), [=] (VkExtensionProperties const& p) {
			                                            return std::strcmp(p.extensionName, ext) == 0;
		                                            } ) == 0
		) {
			throw Error(MSG(err) << "Tried to instantiate device, but it's missing this extension: " << ext);
		}
	}

#ifndef NDEBUG
	{
		VkPhysicalDeviceProperties dev_props;
		vkGetPhysicalDeviceProperties(this->phys_device, &dev_props);
		log::log(MSG(dbg) << "Chosen Vulkan graphics device: " << dev_props.deviceName);
		log::log(MSG(dbg) << "Device extensions:");
		for (auto const& ext : exts) {
			log::log(MSG(dbg) << "\t" << ext.extensionName);
		}
	}
#endif

	// Prepare device creation
	VkDeviceCreateInfo create_dev {};
	create_dev.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_dev.queueCreateInfoCount = q_infos.size();
	create_dev.pQueueCreateInfos = q_infos.data();
	create_dev.enabledExtensionCount = ext_names.size();
	create_dev.ppEnabledExtensionNames = ext_names.data();

	VkPhysicalDeviceFeatures features {};
	// TODO request features
	create_dev.pEnabledFeatures = &features;

	VK_CALL_CHECKED(vkCreateDevice, this->phys_device, &create_dev, nullptr, &this->device);

	// Obtain handles for the created queues
	this->queues.resize(q_fams.size());
	for (size_t i = 0; i < q_fams.size(); i++) {
		vkGetDeviceQueue(this->device, q_fams[i], 0, &this->queues[i]);
	}
}

VkPhysicalDevice VlkGraphicsDevice::get_physical_device() const {
	return this->phys_device;
}

VkDevice VlkGraphicsDevice::get_device() const {
	return this->device;
}

VkQueue VlkGraphicsDevice::get_queue(size_t idx) const {
	return this->queues[idx];
}

VlkGraphicsDevice::~VlkGraphicsDevice() {
	vkDestroyDevice(this->device, nullptr);
}

} // openage::renderer::vulkan
