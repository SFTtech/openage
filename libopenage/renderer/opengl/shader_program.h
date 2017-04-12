// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <map>
#include <vector>
#include <epoxy/gl.h>

#include "../shader_program.h"
#include "../resources/shader_source.h"
#include "../../util/vector.h"
#include "../renderer.h"
#include "uniform_input.h"
#include "context.h"


namespace openage {
namespace renderer {
namespace opengl {

/// GLSL uniform types.
enum class gl_uniform_t {
	I32,
	U32,
	F32,
	F64,
	V2F32,
	V3F32,
	V4F32,
	SAMPLER2D,
};

/// @returns the size in bytes of a GLSL uniform type
size_t uniform_size(gl_uniform_t);

struct GlUniform {
	gl_uniform_t type;
	GLint location;
};

class GlShaderProgram : public ShaderProgram {
public:
	/// Tries to create a shader program from the given sources.
	/// Throws an exception on compile/link errors.
	explicit GlShaderProgram(const std::vector<resources::ShaderSource>&, const gl_context_capabilities&);
	~GlShaderProgram();

	GlShaderProgram(const GlShaderProgram&) = delete;
	GlShaderProgram& operator=(const GlShaderProgram&) = delete;

	GlShaderProgram(GlShaderProgram&&);
	GlShaderProgram& operator=(GlShaderProgram&&);

	/// Bind this program as the currently used one in the OpenGL context.
	void use() const;

	/// Renders the specified renderable using this shader program.
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
	void set_tex(UniformInput*, const char*, Texture const*) override;

private:
	std::map<std::string, GlUniform> uniforms;

	// TODO parse uniform buffer structure ugh
	// std::unordered_map<std::string, ..> uniform_buffers;
	// GlVertexInputInfo;

	/// The GL shader program ID
	GLuint id;

	std::unordered_map<std::string, GLuint> texunits_per_unifs;
	std::unordered_map<GLuint, GLuint> textures_per_texunits;
};

}}} // openage::renderer::opengl
