// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <string>

#include "../../util/path.h"


namespace openage {
namespace renderer {
namespace resources {

/// The source language of a shader.
enum class shader_lang_t {
	glsl,
	spirv,
};

/// Shader stages present in modern graphics pipelines.
enum class shader_stage_t {
	vertex,
	geometry,
	tesselation_control,
	tesselation_evaluation,
	fragment,
};

/// Stores source code for a part of a shader program.
class ShaderSource {
public:
	/// Obtain shader source code from a file.
	ShaderSource(shader_lang_t, shader_stage_t, const util::Path &path);

	/// Obtain shader source code from a string.
	ShaderSource(shader_lang_t, shader_stage_t, std::string &&code);

	/// Returns a view of the shader source code. This might be text
	/// or binary data.
	std::string const& get_source() const;

	/// Returns the language of this shader source.
	shader_lang_t get_lang() const;

	/// Returns the stage which this shader source implements.
	shader_stage_t get_stage() const;

private:
	/// The source language.
	shader_lang_t lang;

	/// The implemented stage.
	shader_stage_t stage;

	/// The shader source code.
	std::string code;
};

}}} // openage::renderer::resources
