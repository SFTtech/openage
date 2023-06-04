// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <unordered_map>
#include <vector>

#include "../renderer.h"
#include "../resources/shader_source.h"
#include "../shader_program.h"

#include "geometry.h"
#include "simple_object.h"
#include "uniform_input.h"


namespace openage {
namespace renderer {
namespace opengl {

class GlContext;

/// A handle to an OpenGL shader program.
class GlShaderProgram final : public ShaderProgram
	, public GlSimpleObject {
public:
	/// Tries to create a shader program from the given sources.
	/// Throws an exception on compile/link errors.
	explicit GlShaderProgram(const std::shared_ptr<GlContext> &,
	                         const std::vector<resources::ShaderSource> &);

	/// Bind this program as the currently used one in the OpenGL context.
	void use();

	/// Check if this program is currently in use in the OpenGL context.
	bool in_use() const;

	/// Updates the uniform values with the given input specification.
	void update_uniforms(std::shared_ptr<GlUniformInput> const &);

	bool has_uniform(const char *) override;

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

	/// Represents a uniform in the default block, i.e. not within a named block.
	struct GlUniform {
		GLenum type;
		/// Location of the uniform for use with glUniform and glGetUniform.
		/// NOT the same as the uniform index.
		GLuint location;
	};

	/// Represents a uniform in a named block.
	struct GlInBlockUniform {
		GLenum type;
		/// Offset from the beginning of the block at which this uniform is placed.
		size_t offset;
		/// The size in bytes of the whole uniform. If the uniform is an array,
		/// the size of the whole array.
		size_t size;
		/// Only relevant for arrays and matrices.
		/// In arrays, specifies the distance between the start of each element.
		/// In row-major matrices, specifies the distance between the start of each row.
		/// In column-major matrices, specifies the distance between the start of each column.
		size_t stride;
		/// Only relevant for arrays. The number of elements in the array.
		size_t count;
	};

	/// Represents a uniform block in the shader program.
	struct GlUniformBlock {
		GLuint index;
		/// Size of the entire block. How uniforms are packed within depends
		/// on the block layout and is described in corresponding GlUniforms.
		size_t data_size;
		/// Maps uniform names within this block to their descriptions.
		std::unordered_map<std::string, GlInBlockUniform> uniforms;

		/// The binding point assigned to this block.
		GLuint binding_point;
	};

	/// Represents a per-vertex input to the shader program.
	struct GlVertexAttrib {
		GLenum type;
		GLint location;
		// TODO what is this?
		GLint size;
	};

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
