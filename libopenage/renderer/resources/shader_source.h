// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <string>
#include <vector>
#include <experimental/string_view>


/** @file
 * gpu program creation classes reside in here.
 */
namespace openage {
namespace renderer {
namespace resources {

/// Available shader types present in modern graphics pipelines.
enum class shader_type {
	glsl_vertex,
	glsl_geometry,
	glsl_tesselation_control,
	glsl_tesselation_evaluation,
	glsl_fragment,
};

/// Shader source code storage.
class ShaderSource {
public:
	/// Obtain shader source code from a file.
	ShaderSource from_file(shader_type, std::experimental::string_view path);

	/// Obtain shader source code from a string.
	ShaderSource from_string(shader_type, std::experimental::string_view code);

	/// @returns the shader source code
	const char* get_source() const;

	/// @returns the shader type
	shader_type get_type() const;

private:
	ShaderSource(shader_type type, std::string &&code);

private:
	/// The type of the source code.
	shader_type type;

	/// Stores the shader source code.
	std::string code;
};

}}} // openage::renderer::resources

