// Copyright 2015-2015 the openage authors. See copying.md for legal info.

/** @file
 * common code for all shaders.
 */

#include "shader.h"

#include "../error/error.h"
#include "../util/file.h"

namespace openage {
namespace renderer {

ShaderSource::ShaderSource(shader_type type)
	:
	type{type} {}


shader_type ShaderSource::get_type() const {
	return this->type;
}


const char *ShaderSource::get_source() const {
	return this->code.c_str();
}


ShaderSourceFile::ShaderSourceFile(shader_type type, const std::string &path)
	:
	ShaderSource{type},
	path{path} {

	this->code = util::read_whole_file(this->path);
}


ShaderSourceCode::ShaderSourceCode(shader_type type, const char *code)
	:
	ShaderSource{type} {

	this->code = code;
}


ShaderSourceCode::ShaderSourceCode(shader_type type, const std::string &code)
	:
	ShaderSource{type} {

	this->code = code;
}

}} // openage::renderer
