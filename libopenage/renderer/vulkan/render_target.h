#pragma once

#include "../renderer.h"

#include "graphics_device.h"


namespace openage {
namespace renderer {
namespace vulkan {

/// A Vulkan representation of a display target that can be drawn onto directly,
/// that is _not_ by copying data from another object.
class VlkDrawableDisplay {
public:
	// use shared_ptr?
	VkDevice device;
	VkSwapchainKHR swapchain;
	VkFormat format;
	VkExtent2D extent;
	std::vector<VkImage> images;
	std::vector<VkImageView> image_views;
	std::vector<VkFramebuffer> framebuffers;

	VkSurfaceFormatKHR choose_best_surface_format(std::vector<VkSurfaceFormatKHR> const& formats) {
		// If the implementation doesn't have preferred formats, choose our own
		if (formats.size() == 1
		    && formats[0].format == VK_FORMAT_UNDEFINED)
		{
			return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		}

		// Otherwise if one of the preferred formats is good, choose that
		for (const auto& fmt : formats) {
			if (fmt.format == VK_FORMAT_B8G8R8_UNORM
			    && fmt.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return fmt;
			}
		}

		// Otherwise select any format
		return formats[0];
	}

	VkPresentModeKHR choose_best_present_mode(std::vector<VkPresentModeKHR> const& modes) {
		VkPresentModeKHR ret = VK_PRESENT_MODE_MAILBOX_KHR;

		for (const auto& mode : modes) {
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return mode;
			} else if (mode == VK_PRESENT_MODE_FIFO_KHR) {
				ret = mode;
			} else if (mode == VK_PRESENT_MODE_FIFO_RELAXED_KHR
			           && ret != VK_PRESENT_MODE_FIFO_KHR)
			{
				ret = mode;
			} else if (mode == VK_PRESENT_MODE_IMMEDIATE_KHR
			           && ret != VK_PRESENT_MODE_FIFO_KHR
			           && ret != VK_PRESENT_MODE_FIFO_RELAXED_KHR)
			{
				ret = mode;
			}
		}

		return ret;
	}

	VlkDrawableDisplay(VkDevice device, SurfaceSupportDetails details)
		: device(device) {
		VkSurfaceFormatKHR format = choose_best_surface_format(details.surface_formats);
		this->format = format.format;

		VkPresentModeKHR mode = choose_best_present_mode(details.present_modes);

		if (details.surface_caps.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			this->extent = details.surface_caps.currentExtent;
		} else {
			// TODO choose a size from this->size in this case
			throw Error(MSG(err) << "Window manager does not provide a window size.");
		}

		uint32_t img_count = details.surface_caps.minImageCount + 1;
		if (details.surface_caps.maxImageCount != 0) {
			img_count = std::max(img_count, details.surface_caps.maxImageCount);
		}

		VkSwapchainCreateInfoKHR cr_swap = {};
		cr_swap.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		cr_swap.surface = details.surface;
		cr_swap.minImageCount = img_count;
		cr_swap.imageFormat = this->format;
		cr_swap.imageColorSpace = format.colorSpace;
		cr_swap.presentMode = mode;
		// TODO why doesn't validation work?
		cr_swap.imageExtent = this->extent;
		cr_swap.imageArrayLayers = 1;
		// or VK_IMAGE_USAGE_TRANSFER_DST_BIT if not drawing directly (postprocess)
		cr_swap.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		if (details.maybe_present_fam) {
			// We have to share the swapchain between different queues in this case
			cr_swap.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			cr_swap.queueFamilyIndexCount = 2;
			std::array<uint32_t, 2> q_fams = {{ details.graphics_fam, *details.maybe_present_fam }};
			cr_swap.pQueueFamilyIndices = q_fams.data();
		} else {
			// Otherwise only one queue can access it
			cr_swap.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		cr_swap.preTransform = details.surface_caps.currentTransform;
		cr_swap.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		// Discard pixels which are not visible
		cr_swap.clipped = VK_TRUE;
		cr_swap.oldSwapchain = VK_NULL_HANDLE;

		VK_CALL_CHECKED(vkCreateSwapchainKHR, this->device, &cr_swap, nullptr, &this->swapchain);

		this->images = vk_do_ritual(vkGetSwapchainImagesKHR, this->device, this->swapchain);

		// TODO move out?
		for (auto img : this->images) {
			VkImageViewCreateInfo cr_view = {};
			cr_view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			cr_view.image = img;
			cr_view.viewType = VK_IMAGE_VIEW_TYPE_2D;
			cr_view.format = this->format;
			cr_view.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			cr_view.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			cr_view.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			cr_view.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			cr_view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			cr_view.subresourceRange.baseMipLevel = 0;
			cr_view.subresourceRange.levelCount = 1;
			cr_view.subresourceRange.baseArrayLayer = 0;
			cr_view.subresourceRange.layerCount = 1;

			VkImageView view;
			VK_CALL_CHECKED(vkCreateImageView, this->device, &cr_view, nullptr, &view);

			this->image_views.push_back(view);
		}
	}

	~VlkDrawableDisplay() {
		vkDestroySwapchainKHR(this->device, this->swapchain, nullptr);
	}
};

class VlkFramebuffer final : public RenderTarget {
	std::vector<VkImageView> attachments;
	VkFramebuffer framebuffer;
	VkViewport viewport;

public:
	VlkFramebuffer(VkRenderPass pass, std::vector<VkImageView> const& attachments) {

	}
};

}}} // openage::renderer::vulkan
