// Copyright 2015-2015 the openage authors. See copying.md for legal info.

/** @file
 * common code for all pipeline programs.
 */

#include "pipeline.h"

namespace openage {
namespace renderer {

PipelineVariable::PipelineVariable(Program *program)
	:
	program{program} {
}

PipelineVariable::~PipelineVariable() {}

void PipelineVariable::set_program(Program *program) {
	this->program = program;
}

void PipelineVariable::set_name(const std::string &name) {
	this->name = name;
}


template<>
void Uniform<Texture>::set_impl(const Texture &value) {
	this->program->set_uniform_2dtexture(this->name.c_str(), value);
}

template<>
void Uniform<int>::set_impl(const int &value) {
	this->program->set_uniform_1i(this->name.c_str(), value);
}


Pipeline::Pipeline(Program *prg)
	:
	program{prg} {
}

Pipeline::~Pipeline() {}


void Pipeline::add_var(const std::string &name, PipelineVariable &var) {
	// just add the variable to the known list
	this->variables[name] = &var;
	var.set_program(this->program);
}


}} // openage::renderer
