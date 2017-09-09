#include "renderer.h"

#include "../../error/error.h"
#include "../../util/path.h"
#include "../../util/fslike/directory.h"

#include "../resources/shader_source.h"

#include "util.h"
#include "graphics_device.h"
#include "render_target.h"
#include "shader_program.h"


namespace openage {
namespace renderer {
namespace vulkan {

void VlkRenderer::do_the_thing() {
	// Enumerate available physical devices
	auto devices = vk_do_ritual(vkEnumeratePhysicalDevices, this->instance);
	if (devices.size() == 0) {
		throw Error(MSG(err) << "No Vulkan devices available.");
	}

	std::vector<std::pair<VkPhysicalDevice, SurfaceSupportDetails>> support_per_dev;
	for (auto dev : devices) {
		auto support = VlkGraphicsDevice::find_device_surface_support(dev, surface);
		if (support) {
			support_per_dev.emplace_back(dev, *support);
		}
	}

	if (support_per_dev.empty()) {
		throw Error(MSG(err) << "No valid Vulkan device available.");
	}

	// TODO rate devices based on capabilities
	// Given an instance and surface, selects the best (fastest, most capable, etc.) graphics device
	// which supports rendering onto that particular surface and constructs the object.

	auto const& info = support_per_dev[0];

	// Create a logical device with a single queue for both graphics and present
	if (info.second.maybe_present_fam) {
		throw 0;
	}

	VlkGraphicsDevice dev(info.first, { info.second.graphics_fam } );

	VlkDrawableDisplay display(dev.get_device(), info.second);

	// TODO reinit swapchain on window resize

	auto dir = std::make_shared<util::fslike::Directory>("/home/wojtek/Programming/C++/openage/");

	auto vert = resources::ShaderSource(
		resources::shader_lang_t::spirv,
		resources::shader_stage_t::vertex,
		util::Path(dir) / "assets/shaders/vert.spv"
	);

	auto frag = resources::ShaderSource(
		resources::shader_lang_t::spirv,
		resources::shader_stage_t::fragment,
		util::Path(dir) / "assets/shaders/frag.spv"
	);

	VlkShaderProgram prog(dev.get_device(), { vert, frag } );

	VkPipelineVertexInputStateCreateInfo cr_vert_in = {};
	cr_vert_in.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	// all init'd to 0

	VkPipelineInputAssemblyStateCreateInfo cr_in_asm = {};
	cr_in_asm.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	cr_in_asm.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	cr_in_asm.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = display.extent.width;
	viewport.height = display.extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = display.extent;

	VkPipelineViewportStateCreateInfo cr_view = {};
	cr_view.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	cr_view.viewportCount = 1;
	cr_view.pViewports = &viewport;
	cr_view.scissorCount = 1;
	cr_view.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo cr_raster = {};
	cr_raster.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	cr_raster.depthClampEnable = VK_FALSE;
	cr_raster.rasterizerDiscardEnable = VK_FALSE;
	cr_raster.polygonMode = VK_POLYGON_MODE_FILL;
	cr_raster.lineWidth = 1.0f;
	cr_raster.cullMode = VK_CULL_MODE_BACK_BIT;
	cr_raster.frontFace = VK_FRONT_FACE_CLOCKWISE;
	cr_raster.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo cr_msaa = {};
	cr_msaa.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	cr_msaa.sampleShadingEnable = VK_FALSE;
	cr_msaa.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState blend_state = {};
	blend_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
	                             | VK_COLOR_COMPONENT_G_BIT
	                             | VK_COLOR_COMPONENT_B_BIT
	                             | VK_COLOR_COMPONENT_A_BIT;
	blend_state.blendEnable = VK_TRUE;
	blend_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	blend_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	blend_state.colorBlendOp = VK_BLEND_OP_ADD;
	blend_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	blend_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	blend_state.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo cr_blend = {};
	cr_blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	cr_blend.logicOpEnable = VK_FALSE;
	cr_blend.attachmentCount = 1;
	cr_blend.pAttachments = &blend_state;

	VkPipelineLayoutCreateInfo cr_layout = {};
	cr_layout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	// empty object

	VkPipelineLayout layout;
	VK_CALL_CHECKED(vkCreatePipelineLayout, dev.get_device(), &cr_layout, nullptr, &layout);

	/// RENDERPASS
	VkAttachmentDescription color_attachment = {};
	color_attachment.format = display.format;
	color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference color_attachment_ref = {};
	color_attachment_ref.attachment = 0;
	color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment_ref;

	VkSubpassDependency dep = {};
	dep.srcSubpass = VK_SUBPASS_EXTERNAL;
	dep.dstSubpass = 0;
	dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dep.srcAccessMask = 0;
	dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo cr_render_pass = {};
	cr_render_pass.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	cr_render_pass.attachmentCount = 1;
	cr_render_pass.pAttachments = &color_attachment;
	cr_render_pass.subpassCount = 1;
	cr_render_pass.pSubpasses = &subpass;
	cr_render_pass.dependencyCount = 1;
	cr_render_pass.pDependencies = &dep;

	VkRenderPass render_pass;
	VK_CALL_CHECKED(vkCreateRenderPass, dev.get_device(), &cr_render_pass, nullptr, &render_pass);
	/// RENDERPASS

	VkGraphicsPipelineCreateInfo cr_pipeline = {};
	cr_pipeline.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	cr_pipeline.stageCount = 2;
	cr_pipeline.pStages = prog.pipeline_stage_infos.data();
	cr_pipeline.pVertexInputState = &cr_vert_in;
	cr_pipeline.pInputAssemblyState = &cr_in_asm;
	cr_pipeline.pViewportState = &cr_view;
	cr_pipeline.pRasterizationState = &cr_raster;
	cr_pipeline.pMultisampleState = &cr_msaa;
	cr_pipeline.pDepthStencilState = nullptr;
	cr_pipeline.pColorBlendState = &cr_blend;
	cr_pipeline.pDynamicState = nullptr;
	cr_pipeline.layout = layout;
	cr_pipeline.renderPass = render_pass;
	cr_pipeline.subpass = 0;
	cr_pipeline.basePipelineHandle = VK_NULL_HANDLE;
	cr_pipeline.basePipelineIndex = -1;

	VkPipeline pipeline;
	VK_CALL_CHECKED(vkCreateGraphicsPipelines, dev.get_device(), VK_NULL_HANDLE, 1, &cr_pipeline, nullptr, &pipeline);

	std::vector<VkFramebuffer> fbufs;
	for (auto view : display.image_views) {
		VkFramebufferCreateInfo cr_fbuf = {};
		cr_fbuf.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		cr_fbuf.renderPass = render_pass;
		cr_fbuf.attachmentCount = 1;
		cr_fbuf.pAttachments = &view;
		cr_fbuf.width = display.extent.width;
		cr_fbuf.height = display.extent.height;
		cr_fbuf.layers = 1;

		VkFramebuffer fbuf;
		VK_CALL_CHECKED(vkCreateFramebuffer, dev.get_device(), &cr_fbuf, nullptr, &fbuf);

		fbufs.push_back(fbuf);
	}

	VkCommandPoolCreateInfo cr_pool = {};
	cr_pool.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cr_pool.queueFamilyIndex = info.second.graphics_fam;
	cr_pool.flags = 0;

	VkCommandPool pool;
	VK_CALL_CHECKED(vkCreateCommandPool, dev.get_device(), &cr_pool, nullptr, &pool);

	std::vector<VkCommandBuffer> cmd_bufs(fbufs.size());
	VkCommandBufferAllocateInfo cr_cmd_bufs = {};
	cr_cmd_bufs.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cr_cmd_bufs.commandPool = pool;
	cr_cmd_bufs.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cr_cmd_bufs.commandBufferCount = static_cast<uint32_t>(cmd_bufs.size());

	VK_CALL_CHECKED(vkAllocateCommandBuffers, dev.get_device(), &cr_cmd_bufs, cmd_bufs.data());

	for (size_t i = 0; i < cmd_bufs.size(); i++) {
		auto cmd_buf = cmd_bufs[i];
		auto fbuf = fbufs[i];

		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		begin_info.pInheritanceInfo = nullptr;

		vkBeginCommandBuffer(cmd_buf, &begin_info);

		VkRenderPassBeginInfo cmd_render = {};
		cmd_render.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		cmd_render.renderPass = render_pass;
		cmd_render.framebuffer = fbuf;
		cmd_render.renderArea.offset = { 0, 0 };
		cmd_render.renderArea.extent = display.extent;

		VkClearValue clear_color = {{{ 0.0f, 0.0f, 0.0f, 1.0f }}};
		cmd_render.clearValueCount = 1;
		cmd_render.pClearValues = &clear_color;

		vkCmdBeginRenderPass(cmd_buf, &cmd_render, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

		vkCmdDraw(cmd_buf, 3, 1, 0, 0);

		vkCmdEndRenderPass(cmd_buf);

		VK_CALL_CHECKED(vkEndCommandBuffer, cmd_buf);

		VkSemaphoreCreateInfo cr_sem = {};
		cr_sem.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkSemaphore sem_image_ready;
		VkSemaphore sem_render_done;
		VK_CALL_CHECKED(vkCreateSemaphore, dev.get_device(), &cr_sem, nullptr, &sem_image_ready);
		VK_CALL_CHECKED(vkCreateSemaphore, dev.get_device(), &cr_sem, nullptr, &sem_render_done);

		uint32_t img_idx = 0;
		VK_CALL_CHECKED(vkAcquireNextImageKHR, dev.get_device(), display.swapchain, std::numeric_limits<uint64_t>::max(), sem_image_ready, VK_NULL_HANDLE, &img_idx);

		VkSubmitInfo submit = {};
		submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit.waitSemaphoreCount = 1;
		submit.pWaitSemaphores = &sem_image_ready;
		VkPipelineStageFlags mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		submit.pWaitDstStageMask = &mask;

		submit.commandBufferCount = 1;
		submit.pCommandBuffers = &cmd_bufs[img_idx];

		submit.signalSemaphoreCount = 1;
		submit.pSignalSemaphores = &sem_render_done;

		VK_CALL_CHECKED(vkQueueSubmit, dev.get_queue(0), 1, &submit, VK_NULL_HANDLE);

		VkPresentInfoKHR present_info = {};
		present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		present_info.waitSemaphoreCount = 1;
		present_info.pWaitSemaphores = &sem_render_done;

		present_info.swapchainCount = 1;
		present_info.pSwapchains = &display.swapchain;
		present_info.pImageIndices = &img_idx;
		present_info.pResults = nullptr;

		vkQueuePresentKHR(dev.get_queue(0), &present_info);

		vkQueueWaitIdle(dev.get_queue(0));

		vkDeviceWaitIdle(dev.get_device());
	}
}

}}} // openage::renderer::vulkan
