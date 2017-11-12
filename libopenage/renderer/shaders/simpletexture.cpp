// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "simpletexture.h"

namespace openage {
namespace renderer {

SimpleTextureMaterial::SimpleTextureMaterial(Program *prg)
	:
	Material{prg},
	tex{"tex", this},
	position{"position", this},
	texcoord{"texcoord", this} {

	// register the variables to the "active" list
	this->add_var(&this->tex);
	this->add_var(&this->position);
	this->add_var(&this->texcoord);
}

void SimpleTextureMaterial::set_positions(mesh_t positions) {
	this->position.set(positions);
}


}} // openage::renderer
