// Copyright 2015-2017 the openage authors. See copying.md for legal info.

/** @file
 * common code for all shaders.
 */

#include "shader_source.h"

#include "../../error/error.h"
#include "../../util/file.h"

namespace openage {
namespace renderer {
namespace resources {

ShaderSource::ShaderSource(shader_type type, std::string &&code)
	: type(type)
	, code(std::move(code)) {}

ShaderSource ShaderSource::from_file(shader_type type, std::experimental::string_view path) {
	return ShaderSource(type, util::read_whole_file(path.data()));
}

ShaderSource ShaderSource::from_string(shader_type type, std::experimental::string_view code) {
	return ShaderSource(type, std::string(code));
}

shader_type ShaderSource::get_type() const {
	return this->type;
}

const char* ShaderSource::get_source() const {
	return this->code.c_str();
}

}}} // openage::renderer::resources
