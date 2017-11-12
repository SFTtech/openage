// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "shader_source.h"

#include "../../util/file.h"


namespace openage {
namespace renderer {
namespace resources {
const char *shader_source_type_to_str(shader_source_t type) {
	switch (type) {
	case shader_source_t::glsl_vertex:
		return "vertex shader";
	case shader_source_t::glsl_geometry:
		return "geometry shader";
	case shader_source_t::glsl_tesselation_control:
		return "tesselation control shader";
	case shader_source_t::glsl_tesselation_evaluation:
		return "tesselation evaluation shader";
	case shader_source_t::glsl_fragment:
		return "fragment shader";
	default:
		return "unknown shader type";
	}
}

ShaderSource::ShaderSource(shader_source_t type, std::string &&code)
	: _type(type)
	, code(std::move(code)) {}

ShaderSource ShaderSource::from_file(shader_source_t type, const char *path) {
	util::File file(path);
	return ShaderSource(type, file.read());
}

ShaderSource ShaderSource::from_string(shader_source_t type, std::string &&code) {
	return ShaderSource(type, std::move(code));
}

shader_source_t ShaderSource::type() const {
	return this->_type;
}

const char *ShaderSource::source() const {
	return this->code.data();
}

}}} // openage::renderer::resources
