// Copyright 2015-2023 the openage authors. See copying.md for legal info.

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

	bool has_uniform(const char *) override;

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

private:
	void set_unif(std::shared_ptr<UniformInput> const &, const char *, void const *, GLenum);

	/// Maps uniform names to their descriptions. Contains only
	/// uniforms in the default block, i.e. not within named blocks.
	std::unordered_map<std::string, GlUniform> uniforms;

	/// Maps uniform block names to their descriptions.
	std::unordered_map<std::string, GlUniformBlock> uniform_blocks;

	/// Maps per-vertex attribute names to their descriptions.
	std::unordered_map<std::string, GlVertexAttrib> attribs;

	/// Maps sampler uniform names to their assigned texture units.
	std::unordered_map<std::string, GLuint> texunits_per_unifs;
	/// Maps texture units to the texture handles that are currently bound to them.
	std::unordered_map<GLuint, GLuint> textures_per_texunits;

	/// Whether this program has been validated.
	bool validated;
};

} // namespace opengl
} // namespace renderer
} // namespace openage
