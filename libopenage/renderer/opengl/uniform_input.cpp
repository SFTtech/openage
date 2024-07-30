// Copyright 2019-2024 the openage authors. See copying.md for legal info.

#include "uniform_input.h"

#include "renderer/opengl/shader_program.h"
#include "renderer/opengl/util.h"


namespace openage::renderer::opengl {

GlUniformInput::GlUniformInput(const std::shared_ptr<ShaderProgram> &prog) :
	UniformInput{prog} {
	auto glprog = std::dynamic_pointer_cast<GlShaderProgram>(prog);

	// Calculate the byte-wise offsets of all uniforms.
	size_t offset = 0;
	this->update_offs.reserve(glprog->get_uniforms().size());
	for (auto &uniform : glprog->get_uniforms()) {
		this->update_offs.push_back({offset, false});
		offset += get_uniform_type_size(uniform.type);
	}

	// Resize the update data buffer to the total size of all uniforms.
	this->update_data.resize(offset);
}

GlUniformBufferInput::GlUniformBufferInput(std::shared_ptr<UniformBuffer> const &buffer) :
	UniformBufferInput{buffer} {}

} // namespace openage::renderer::opengl
