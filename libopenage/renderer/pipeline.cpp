// Copyright 2015-2015 the openage authors. See copying.md for legal info.

/** @file
 * common code for all pipeline programs.
 */

#include "pipeline.h"

namespace openage {
namespace renderer {

Pipeline::Pipeline(Program *prg)
	:
	program{prg} {
}

Pipeline::~Pipeline() {}


void Pipeline::add_var(const std::string &name, PipelineVariable &var) {
	// just add the variable to the known list
	this->variables[name] = &var;
}


}} // openage::renderer
