// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <optional>
#include <vector>

#include <vulkan/vulkan.h>


namespace openage {
namespace renderer {
namespace vulkan {

/// Contains information about the support of a given physical device for a given surface,
/// for example the formats using which it can present onto the surface.
struct SurfaceSupportDetails {
	/// The physical device.
	VkPhysicalDevice phys_device;

	/// The surface to which it can draw.
	VkSurfaceKHR surface;

	/// Available present modes.
	std::vector<VkPresentModeKHR> present_modes;

	/// Available surface formats.
	std::vector<VkSurfaceFormatKHR> surface_formats;

	/// Various capabilities of presentation to the surface.
	VkSurfaceCapabilitiesKHR surface_caps;

	/// Index of the device queue family with graphics support.
	uint32_t graphics_fam;

	/// Index of the queue family with presentation support. This might be the same as the graphics
	/// family, in which case this optional is empty.
	std::optional<uint32_t> maybe_present_fam;
};

/// Owns a device capable of graphics operations and surface presentation using WSI.
class VlkGraphicsDevice {
	/// The underlying physical device.
	VkPhysicalDevice phys_device;

	/// Logical device, owned by this object.
	VkDevice device;

	/// The queues instantiated for this device.
	std::vector<VkQueue> queues;

public:
	/// Given a physical device and a surface, checks whether the device is capable of presenting to the surface.
	/// If it is, returns information about its presentation capabilities, otherwise returns an empty optional.
	static std::optional<SurfaceSupportDetails> find_device_surface_support(VkPhysicalDevice, VkSurfaceKHR);

	/// Given a physical device and a list of queue family indices in that device, instantiates
	/// a logical device with a queue per each of the families. The device has to support the
	/// swapchain extension.
	VlkGraphicsDevice(VkPhysicalDevice dev, std::vector<uint32_t> const& q_fams);

	VkPhysicalDevice get_physical_device() const;
	VkDevice get_device() const;
	VkQueue get_queue(size_t idx) const;

	// \todo structure isn't ideal, maybe store SurfaceSupportDetails in here?

	~VlkGraphicsDevice();
};

}}} // openage::renderer::vulkan
