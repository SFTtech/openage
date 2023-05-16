// Copyright 2019-2023 the openage authors. See copying.md for legal info.

#include "uniform_input.h"

namespace openage::renderer::opengl {

GlUniformInput::GlUniformInput(std::shared_ptr<ShaderProgram> const &prog) :
	UniformInput{prog} {}

GlUniformBufferInput::GlUniformBufferInput(std::shared_ptr<UniformBuffer> const &buffer) :
	UniformBufferInput{buffer} {}

} // namespace openage::renderer::opengl
