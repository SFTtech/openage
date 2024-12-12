// Copyright 2019-2024 the openage authors. See copying.md for legal info.

#include "uniform_input.h"

#include "renderer/opengl/lookup.h"
#include "renderer/opengl/shader_program.h"
#include "renderer/opengl/uniform_buffer.h"
#include "renderer/opengl/util.h"


namespace openage::renderer::opengl {

GlUniformInput::GlUniformInput(const std::shared_ptr<ShaderProgram> &prog) :
	UniformInput{prog} {
	auto glprog = std::dynamic_pointer_cast<GlShaderProgram>(prog);

	// Reserve space for the used uniforms.
	this->used_uniforms.reserve(glprog->get_uniforms().size());

	// Calculate the byte-wise offsets of all uniforms.
	size_t offset = 0;
	this->update_offs.reserve(glprog->get_uniforms().size());
	for (auto &uniform : glprog->get_uniforms()) {
		this->update_offs.push_back({offset, false});
		offset += GL_UNIFORM_TYPE_SIZE.get(uniform.type);
	}

	// Resize the update data buffer to the total size of all uniforms.
	this->update_data.resize(offset);
}

GlUniformBufferInput::GlUniformBufferInput(const std::shared_ptr<UniformBuffer> &buffer) :
	UniformBufferInput{buffer} {
	auto glBuf = std::dynamic_pointer_cast<GlUniformBuffer>(buffer);

	auto uniforms = glBuf->get_uniforms();

	// Reserve space for the used uniforms.
	this->used_uniforms.reserve(uniforms.size());

	// Calculate the byte-wise offsets of all uniforms.
	size_t offset = 0;
	this->update_offs.reserve(uniforms.size());
	for (auto &uniform : uniforms) {
		this->update_offs.push_back({uniform.offset, false});
		offset += GL_UNIFORM_TYPE_SIZE.get(uniform.type);
	}

	// Resize the update data buffer to the total size of all uniforms.
	this->update_data.resize(offset);
}

bool GlUniformInput::is_complete() const {
	for (const auto &uniform : this->update_offs) {
		if (not uniform.used) {
			return false;
		}
	}
	return true;
}

} // namespace openage::renderer::opengl
