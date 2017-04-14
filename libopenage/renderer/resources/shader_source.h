// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <string>

#include "../../util/path.h"


namespace openage {
namespace renderer {
namespace resources {

/// Available shader types present in modern graphics pipelines.
/// Contains information about the source language and the shader
/// stage that it describes.
enum class shader_source_t {
	glsl_vertex,
	glsl_geometry,
	glsl_tesselation_control,
	glsl_tesselation_evaluation,
	glsl_fragment,
};

/// Returns a readable description of the given shader source type.
const char *shader_source_type_to_str(shader_source_t);

/// Stores source code for a part of a shader program.
class ShaderSource {
public:
	/// Obtain shader source code from a file.
	ShaderSource(shader_source_t, const util::Path &path);

	/// Obtain shader source code from a string.
	ShaderSource(shader_source_t, std::string &&code);

	/// Returns a view of the shader source code
	const char *get_source() const;

	/// Returns the type of this shader source.
	shader_source_t get_type() const;

private:
	/// The type of the source.
	shader_source_t type;

	/// The shader source code.
	std::string code;
};

}}} // openage::renderer::resources
