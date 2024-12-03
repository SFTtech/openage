// Copyright 2015-2024 the openage authors. See copying.md for legal info.

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
class GlUniformBuffer;

/**
 * Describes OpenGL-specific uniform valuations.
 */
class GlUniformInput final : public UniformInput {
private:
	struct GlUniformOffset {
		// Offset in the update_data buffer.
		size_t offset;
		/// Dtermine whether the uniform value has been set.
		bool used;
	};

public:
	GlUniformInput(const std::shared_ptr<ShaderProgram> &prog);

	/**
	 * Check if all uniforms have been set.
	 */
	bool is_complete() const override;

	/**
	 * Store the IDs of the uniforms from the shader set by this uniform input.
	 */
	std::vector<uniform_id_t> used_uniforms;

	/**
	 * Store offsets of uniforms in the update_data buffer and
	 * whether the uniform value has been set.
	 *
	 * Index in the vector corresponds to the uniform ID in the shader.
	 */
	std::vector<GlUniformOffset> update_offs;

	/**
	 * Buffer containing untyped uniform update data.
	 */
	std::vector<uint8_t> update_data;
};

/**
 * Describes OpenGL-specific uniform buffer valuations.
 */
class GlUniformBufferInput final : public UniformBufferInput {
private:
	struct GlUniformOffset {
		// Offset in the update_data buffer.
		size_t offset;
		/// Dtermine whether the uniform value has been set.
		bool used;
	};


public:
	GlUniformBufferInput(const std::shared_ptr<UniformBuffer> &buffer);

	/**
	 * Store the IDs of the uniforms from the shader set by this uniform input.
	 */
	std::vector<uniform_id_t> used_uniforms;

	/**
	 * Store offsets of uniforms in the update_data buffer and
	 * whether the uniform value has been set.
	 */
	std::vector<GlUniformOffset> update_offs;

	/**
	 * Buffer containing untyped uniform update data.
	 */
	std::vector<uint8_t> update_data;
};

} // namespace opengl
} // namespace renderer
} // namespace openage
