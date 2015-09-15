// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "simpletexture.h"

namespace openage {
namespace renderer {

SimpleTexturePipeline::SimpleTexturePipeline(Program *prg)
	:
	Pipeline{prg} {

	this->add_var("tex", this->tex);
	this->add_var("position", this->position);
	this->add_var("texcoord", this->texcoord);
}

SimpleTexturePipeline::~SimpleTexturePipeline() {}

}} // openage::renderer
