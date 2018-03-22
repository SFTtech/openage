// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <unordered_map>
#include <vector>

#include "../shader_program.h"
#include "../resources/shader_source.h"
#include "../renderer.h"

#include "uniform_input.h"
#include "context.h"
#include "geometry.h"
#include "simple_object.h"


namespace openage {
namespace renderer {
namespace opengl {

/// A handle to an OpenGL shader program.
class GlShaderProgram final : public ShaderProgram, public GlSimpleObject {
public:
	/// Tries to create a shader program from the given sources.
	/// Throws an exception on compile/link errors.
	explicit GlShaderProgram(const std::vector<resources::ShaderSource>&, const gl_context_capabilities&);

	/// Bind this program as the currently used one in the OpenGL context.
	void use() const;

	/// Does what the description of Renderable specifies - updates the uniform values
	/// and draws the Geometry if it's not nullptr.
	void execute_with(const GlUniformInput*, const GlGeometry*);

	bool has_uniform(const char*) override;

protected:
	std::shared_ptr<UniformInput> new_unif_in() override;
	void set_i32(UniformInput*, const char*, int32_t) override;
	void set_u32(UniformInput*, const char*, uint32_t) override;
	void set_f32(UniformInput*, const char*, float) override;
	void set_f64(UniformInput*, const char*, double) override;
	void set_v2f32(UniformInput*, const char*, Eigen::Vector2f const&) override;
	void set_v3f32(UniformInput*, const char*, Eigen::Vector3f const&) override;
	void set_v4f32(UniformInput*, const char*, Eigen::Vector4f const&) override;
	void set_m4f32(UniformInput*, const char*, Eigen::Matrix4f const&) override;
	void set_tex(UniformInput*, const char*, Texture const*) override;

private:
	void set_unif(UniformInput*, const char*, void const*, GLenum);

	/// Represents a uniform location in the shader program.
	struct GlUniform {
		GLenum type;
		GLint location;
		/// For arrays, the number of elements. For scalars, 1.
		size_t count;
		/// The size in bytes of the whole uniform (whole array if it's one).
		size_t size;
	};

	/// Represents a per-vertex input to the shader program.
	struct GlVertexAttrib {
		GLenum type;
		GLint location;
		// TODO what is this?
		GLint size;
	};

	/// A map of uniform names to their descriptions.
	std::unordered_map<std::string, GlUniform> uniforms;

	/// A map of per-vertex attribute names to their descriptions.
	std::unordered_map<std::string, GlVertexAttrib> attribs;

	// TODO parse uniform buffer structure ugh
	// std::unordered_map<std::string, ..> uniform_buffers;
	// GlVertexInputInfo;

	/// A map from sampler uniform names to their assigned texture units.
	std::unordered_map<std::string, GLuint> texunits_per_unifs;
	/// A map from texture units to the texture handles that are currently bound to them.
	std::unordered_map<GLuint, GLuint> textures_per_texunits;
};

}}} // openage::renderer::opengl
