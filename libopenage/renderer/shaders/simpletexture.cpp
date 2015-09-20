// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "simpletexture.h"

namespace openage {
namespace renderer {

SimpleTexturePipeline::SimpleTexturePipeline(Program *prg)
	:
	Pipeline{prg},
	tex{"tex", this},
	position{"position", this},
	texcoord{"texcoord", this} {

	// we can't register the variables in the constructor of a
	// PipelineVariable, as this would store the wrong type.
	this->add_var(&this->tex);
	this->add_var(&this->position);
	this->add_var(&this->texcoord);
}

}} // openage::renderer
