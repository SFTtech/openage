// Copyright 2015-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <unordered_map>
#include <vector>

#include "renderer/opengl/shader_data.h"
#include "renderer/opengl/simple_object.h"
#include "renderer/renderer.h"
#include "renderer/resources/shader_source.h"
#include "renderer/shader_program.h"


namespace openage {
namespace renderer {

class UniformInput;

namespace opengl {

class GlContext;
class GlUniformBuffer;
class GlUniformInput;

/**
 * A handle to an OpenGL shader program.
 */
class GlShaderProgram final : public ShaderProgram
	, public GlSimpleObject {
public:
	/**
	 * Tries to create a shader program from the given sources.
	 * Throws an exception on compile/link errors.
	 */
	explicit GlShaderProgram(const std::shared_ptr<GlContext> &,
	                         const std::vector<resources::ShaderSource> &);

	/**
	 * Bind this program as the currently used one in the OpenGL context.
	 */
	void use();

	/**
	 * Check if this program is currently in use in the OpenGL context.
	 *
	 * @return true if the program is in use, false otherwise.
	 */
	bool in_use() const;

	/**
	 * Updates the uniform values with the given input specification.
	 *
	 * @param input The uniform input specification.
	 */
	void update_uniforms(std::shared_ptr<GlUniformInput> const &unif_in);

	/**
	 * Get the uniform block with the given name.
	 *
	 * @param block_name Name of the uniform block.
	 *
	 * @return Uniform block.
	 */
	const GlUniformBlock &get_uniform_block(const char *block_name) const;

	/**
	 * Get the uniform ID for the given uniform name.
	 *
	 * @param name Name of the uniform in the shader code.
	 *
	 * @return ID of the uniform.
	 */
	uniform_id_t get_uniform_id(const char *name) override;

	/**
	 * Get the uniforms in the default block of the shader program.
	 * This does not include uniforms in blocks.
	 *
	 * @return Uniforms in the shader program.
	 */
	const std::vector<GlUniform> &get_uniforms() const;

	/**
	 * Get the map of uniform blocks in the shader program.
	 *
	 * @return Uniform blocks in the shader program.
	 */
	const std::unordered_map<std::string, GlUniformBlock> &get_uniform_blocks() const;

	/**
	 * Check whether the shader program contains a uniform variable with the given name.
	 *
	 * @param name Name of the uniform in the shader code.
	 *
	 * @return true if the shader program contains the uniform, false otherwise.
	 */
	bool has_uniform(const char *name) override;

	/**
	 * Binds a uniform block in the shader program to the same binding point as
	 * the given uniform buffer.
	 *
	 * @param buffer Uniform buffer to bind.
	 * @param block_name Name of the uniform block in the shader program.
	 */
	void bind_uniform_buffer(const char *block_name,
	                         std::shared_ptr<UniformBuffer> const &) override;

	std::map<size_t, resources::vertex_input_t> vertex_attributes() const override;

protected:
	std::shared_ptr<UniformInput> new_unif_in() override;
	void set_i32(std::shared_ptr<UniformInput> const &, const char *, int32_t) override;
	void set_u32(std::shared_ptr<UniformInput> const &, const char *, uint32_t) override;
	void set_f32(std::shared_ptr<UniformInput> const &, const char *, float) override;
	void set_f64(std::shared_ptr<UniformInput> const &, const char *, double) override;
	void set_bool(std::shared_ptr<UniformInput> const &, const char *, bool) override;
	void set_v2f32(std::shared_ptr<UniformInput> const &, const char *, Eigen::Vector2f const &) override;
	void set_v3f32(std::shared_ptr<UniformInput> const &, const char *, Eigen::Vector3f const &) override;
	void set_v4f32(std::shared_ptr<UniformInput> const &, const char *, Eigen::Vector4f const &) override;
	void set_v2i32(std::shared_ptr<UniformInput> const &, const char *, Eigen::Vector2i const &) override;
	void set_v3i32(std::shared_ptr<UniformInput> const &, const char *, Eigen::Vector3i const &) override;
	void set_v4i32(std::shared_ptr<UniformInput> const &, const char *, Eigen::Vector4i const &) override;
	void set_v2ui32(std::shared_ptr<UniformInput> const &, const char *, Eigen::Vector2<uint32_t> const &) override;
	void set_v3ui32(std::shared_ptr<UniformInput> const &, const char *, Eigen::Vector3<uint32_t> const &) override;
	void set_v4ui32(std::shared_ptr<UniformInput> const &, const char *, Eigen::Vector4<uint32_t> const &) override;
	void set_m4f32(std::shared_ptr<UniformInput> const &, const char *, Eigen::Matrix4f const &) override;
	void set_tex(std::shared_ptr<UniformInput> const &, const char *, std::shared_ptr<Texture2d> const &) override;

	void set_i32(std::shared_ptr<UniformInput> const &, const uniform_id_t &, int32_t) override;
	void set_u32(std::shared_ptr<UniformInput> const &, const uniform_id_t &, uint32_t) override;
	void set_f32(std::shared_ptr<UniformInput> const &, const uniform_id_t &, float) override;
	void set_f64(std::shared_ptr<UniformInput> const &, const uniform_id_t &, double) override;
	void set_bool(std::shared_ptr<UniformInput> const &, const uniform_id_t &, bool) override;
	void set_v2f32(std::shared_ptr<UniformInput> const &, const uniform_id_t &, Eigen::Vector2f const &) override;
	void set_v3f32(std::shared_ptr<UniformInput> const &, const uniform_id_t &, Eigen::Vector3f const &) override;
	void set_v4f32(std::shared_ptr<UniformInput> const &, const uniform_id_t &, Eigen::Vector4f const &) override;
	void set_v2i32(std::shared_ptr<UniformInput> const &, const uniform_id_t &, Eigen::Vector2i const &) override;
	void set_v3i32(std::shared_ptr<UniformInput> const &, const uniform_id_t &, Eigen::Vector3i const &) override;
	void set_v4i32(std::shared_ptr<UniformInput> const &, const uniform_id_t &, Eigen::Vector4i const &) override;
	void set_v2ui32(std::shared_ptr<UniformInput> const &, const uniform_id_t &, Eigen::Vector2<uint32_t> const &) override;
	void set_v3ui32(std::shared_ptr<UniformInput> const &, const uniform_id_t &, Eigen::Vector3<uint32_t> const &) override;
	void set_v4ui32(std::shared_ptr<UniformInput> const &, const uniform_id_t &, Eigen::Vector4<uint32_t> const &) override;
	void set_m4f32(std::shared_ptr<UniformInput> const &, const uniform_id_t &, Eigen::Matrix4f const &) override;
	void set_tex(std::shared_ptr<UniformInput> const &, const uniform_id_t &, std::shared_ptr<Texture2d> const &) override;

private:
	/**
	 * Set the uniform value via uniform name from a uniform input.
	 *
	 * This method should only be used for debugging and not for performance-critical code.
	 * String lookups are much slower than using the uniform ID.
	 * If performance is important, use the alternative \p set_unif(..) implementation
	 * that works on IDs instead.
	 *
	 * @param unif_in Uniform input.
	 * @param name Name of the uniform.
	 * @param value Value to set.
	 * @param type Type of the value.
	 */
	void set_unif(std::shared_ptr<UniformInput> const &, const char *, void const *, GLenum);

	/**
	 * Set the uniform value via uniform ID from a uniform input.
	 *
	 * @param unif_in Uniform input.
	 * @param id ID of the uniform.
	 * @param value Value to set.
	 * @param type Type of the value.
	 */
	void set_unif(std::shared_ptr<UniformInput> const &, const uniform_id_t &, void const *, GLenum);

	/// Uniforms in the shader program. Contains only
	/// uniforms in the default block, i.e. not within named blocks.
	std::vector<GlUniform> uniforms;

	/// Maps uniform names to their ID (the index in the uniform vector).
	std::unordered_map<std::string, uniform_id_t> uniforms_by_name;

	/// Maps uniform block names to their descriptions.
	std::unordered_map<std::string, GlUniformBlock> uniform_blocks;

	/// Maps per-vertex attribute names to their descriptions.
	std::unordered_map<std::string, GlVertexAttrib> attribs;

	/// Maps sampler uniform names to their assigned texture units.
	std::unordered_map<uniform_id_t, GLuint> texunits_per_unifs;

	/// Store which texture handles are currently bound to the shader's texture units.
	std::vector<TexunitBinding> textures_per_texunits;

	/// Whether this program has been validated.
	bool validated;
};

} // namespace opengl
} // namespace renderer
} // namespace openage
