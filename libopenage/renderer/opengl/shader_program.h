// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <map>
#include <vector>
#include <epoxy/gl.h>

#include "../shader_program.h"
#include "../resources/shader_source.h"
#include "../renderer.h"
#include "uniform_input.h"
#include "context.h"


namespace openage {
namespace renderer {
namespace opengl {

/// GLSL uniform types
enum class gl_uniform_t {
	I32,
	U32,
	F32,
	F64,
	V2F32,
	V3F32,
	V4F32,
	M4F32,
	SAMPLER2D,
};

/// Returns the size in bytes of a GLSL uniform type
size_t uniform_size(gl_uniform_t);

/// Represents a uniform location in the shader program
struct GlUniform {
	gl_uniform_t type;
	GLint location;
};

/// A handle to an OpenGL shader program
class GlShaderProgram : public ShaderProgram {
public:
	/// Tries to create a shader program from the given sources.
	/// Throws an exception on compile/link errors.
	explicit GlShaderProgram(const std::vector<resources::ShaderSource>&, const gl_context_capabilities&);
	~GlShaderProgram();

	/// No copying.
	GlShaderProgram(const GlShaderProgram&) = delete;
	GlShaderProgram& operator=(const GlShaderProgram&) = delete;

	/// Moving is allowed.
	GlShaderProgram(GlShaderProgram&&);
	GlShaderProgram& operator=(GlShaderProgram&&);

	/// Bind this program as the currently used one in the OpenGL context.
	void use() const;

	/// Does what the description of Renderable specifies - updates the uniform values
	/// and draws the Geometry of it's not nullptr.
	void execute_with(const GlUniformInput*, const Geometry*);

	bool has_uniform(const char*) override;

protected:
	void set_unif(UniformInput*, const char*, void const*);
	std::unique_ptr<UniformInput> new_unif_in() override;
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
	/// A map of uniform locations from their names
	std::map<std::string, GlUniform> uniforms;

	// TODO parse uniform buffer structure ugh
	// std::unordered_map<std::string, ..> uniform_buffers;
	// GlVertexInputInfo;

	/// The GL shader program handle
	GLuint id;

	/// A map from sampler uniform names to their assigned texture units
	std::unordered_map<std::string, GLuint> texunits_per_unifs;
	/// A map from texture units to the texture handles that are currently bound to them
	std::unordered_map<GLuint, GLuint> textures_per_texunits;
};

}}} // openage::renderer::opengl
