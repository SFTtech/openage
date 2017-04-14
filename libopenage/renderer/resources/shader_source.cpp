// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "shader_source.h"

#include "../../util/file.h"


namespace openage {
namespace renderer {
namespace resources {
const char *shader_source_type_to_str(shader_source_t type) {
	switch (type) {
	case shader_source_t::glsl_vertex:
		return "GLSL vertex shader";
	case shader_source_t::glsl_geometry:
		return "GLSL geometry shader";
	case shader_source_t::glsl_tesselation_control:
		return "GLSL tesselation control shader";
	case shader_source_t::glsl_tesselation_evaluation:
		return "GLSL tesselation evaluation shader";
	case shader_source_t::glsl_fragment:
		return "GLSL fragment shader";
	default:
		return "unknown GLSL shader type";
	}
}

ShaderSource::ShaderSource(shader_source_t type, std::string &&code)
	: type(type)
	, code(code) {}

ShaderSource::ShaderSource(shader_source_t type, const util::Path &path)
	: type(type)
	, code(path.open().read()) {}

shader_source_t ShaderSource::get_type() const {
	return this->type;
}

const char *ShaderSource::get_source() const {
	return this->code.data();
}

}}} // openage::renderer::resources
