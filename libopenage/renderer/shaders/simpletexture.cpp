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
}

SimpleTexturePipeline::~SimpleTexturePipeline() {}

}} // openage::renderer
