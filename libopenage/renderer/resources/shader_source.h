// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <string>
#include <experimental/string_view>


namespace openage {
namespace renderer {
namespace resources {

/// Available shader types present in modern graphics pipelines.
/// Also contains information about the source language.
enum class shader_source_t {
	glsl_vertex,
	glsl_geometry,
	glsl_tesselation_control,
	glsl_tesselation_evaluation,
	glsl_fragment,
};

std::experimental::string_view shader_source_type_to_str(shader_source_t);

class ShaderSource {
public:
	/// Obtain shader source code from a file.
	static ShaderSource from_file(shader_source_t, std::experimental::string_view path);

	/// Obtain shader source code from a string.
	static ShaderSource from_string(shader_source_t, std::string &&code);

	/// @returns a view of the shader source code
	std::experimental::string_view source() const;

	shader_source_t type() const;

private:
	ShaderSource(shader_source_t type, std::string &&code);

private:
	shader_source_t _type;

	/// The shader source code.
	std::string code;
};

}}} // openage::renderer::resources
