// Copyright 2017-2018 the openage authors. See copying.md for legal info.

#pragma once

#include "../../error/error.h"
#include "../../log/log.h"

#include "../resources/shader_source.h"
#include "../shader_program.h"


namespace openage {
namespace renderer {
namespace vulkan {

static VkShaderStageFlagBits vk_shader_stage(resources::shader_stage_t stage) {
	switch (stage) {
	case resources::shader_stage_t::vertex: return VK_SHADER_STAGE_VERTEX_BIT;
	case resources::shader_stage_t::geometry: return VK_SHADER_STAGE_GEOMETRY_BIT;
	case resources::shader_stage_t::tesselation_control: return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	case resources::shader_stage_t::tesselation_evaluation: return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	case resources::shader_stage_t::fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
	default: throw Error(MSG(err) << "Unknown shader stage.");
	}
}

class VlkShaderProgram /* final : public ShaderProgram */ {
public:
	std::vector<VkShaderModule> modules;
	std::vector<VkPipelineShaderStageCreateInfo> pipeline_stage_infos;

	explicit VlkShaderProgram(VkDevice dev, std::vector<resources::ShaderSource> const& srcs) {
		// TODO reflect with spirv-cross
		// TODO if glsl, compile to spirv with libshaderc

		for (auto const& src : srcs) {
			if (src.get_lang() != resources::shader_lang_t::spirv) {
				throw Error(MSG(err) << "Unsupported shader language in Vulkan shader.");
			}

			VkShaderModuleCreateInfo cr_shdr = {};
			cr_shdr.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			cr_shdr.codeSize = src.get_source().size();
			cr_shdr.pCode = reinterpret_cast<uint32_t const*>(src.get_source().data());

			VkShaderModule mod;
			VK_CALL_CHECKED(vkCreateShaderModule, dev, &cr_shdr, nullptr, &mod);

			this->modules.push_back(mod);

			VkPipelineShaderStageCreateInfo cr_stage = {};
			cr_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			cr_stage.stage = vk_shader_stage(src.get_stage());
			cr_stage.module = mod;
			cr_stage.pName = "main";

			this->pipeline_stage_infos.push_back(cr_stage);
		}

		log::log(MSG(dbg) << "Created modules for Vulkan shader");
	}
};

}}} // openage::renderer::vulkan
