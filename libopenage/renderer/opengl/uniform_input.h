// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <unordered_map>
#include <vector>

#include "renderer/renderer.h"
#include "renderer/types.h"
#include "renderer/uniform_input.h"


namespace openage {
namespace renderer {
class ShaderProgram;
class UniformBuffer;

namespace opengl {

class GlShaderProgram;

/**
 * Describes OpenGL-specific uniform valuations.
 */
class GlUniformInput final : public UniformInput {
public:
	GlUniformInput(std::shared_ptr<ShaderProgram> const &);

	/**
     * We store uniform updates lazily. They are only actually uploaded to GPU
	 * when a draw call is made.
     *
     * \p update_offs maps the uniform IDs to where their
	 * value is in \p update_data in terms of a byte-wise offset. This is only a partial
	 * valuation, so not all uniforms have to be present here.
     */
	std::unordered_map<uniform_id_t, size_t> update_offs;

	/**
     * Buffer containing untyped uniform update data.
     */
	std::vector<uint8_t> update_data;
};

/**
 * Describes OpenGL-specific uniform buffer valuations.
 */
class GlUniformBufferInput final : public UniformBufferInput {
public:
	GlUniformBufferInput(std::shared_ptr<UniformBuffer> const &);

	/**
     * We store uniform updates lazily. They are only actually uploaded to GPU
	 * when a draw call is made.
     *
     * \p update_offs maps the uniform names to where their
	 * value is in \p update_data in terms of a byte-wise offset. This is only a partial
	 * valuation, so not all uniforms have to be present here.
     */
	std::unordered_map<std::string, size_t> update_offs;

	/**
     * Buffer containing untyped uniform update data.
     */
	std::vector<uint8_t> update_data;
};

} // namespace opengl
} // namespace renderer
} // namespace openage
