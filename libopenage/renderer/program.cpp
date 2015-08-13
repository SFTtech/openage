// Copyright 2015-2015 the openage authors. See copying.md for legal info.

/** @file
 * common code for all pipeline programs.
 */

#include "program.h"

namespace openage {
namespace renderer {

ProgramSource::ProgramSource() {}


ProgramSource::ProgramSource(const std::vector<const ShaderSource *> &shaders) {
	for (auto &shader : shaders) {
		this->attach_shader(*shader);
	}
}


const std::vector<const ShaderSource *> &ProgramSource::get_shaders() const {
	return this->shaders;
}


void ProgramSource::attach_shader(const ShaderSource &shader) {
	this->shaders.push_back(&shader);
}


RawProgram::RawProgram() {}


}} // openage::renderer
