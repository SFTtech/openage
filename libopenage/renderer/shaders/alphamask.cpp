// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "alphamask.h"

namespace openage {
namespace renderer {

AlphamaskMaterial::AlphamaskMaterial(Program *prg)
	:
	Material{prg},
	position{"position", this},
	base_texture{"base_tex", this},
	mask_texture{"mask_tex", this},
	show_mask{"show_mask", this},
	base_tex_coordinates{"base_tex_coordinates", this},
	mask_tex_coordinates{"mask_tex_coordinates", this} {

	this->add_var(&this->position);
	this->add_var(&this->base_texture);
	this->add_var(&this->mask_texture);
	this->add_var(&this->show_mask);
	this->add_var(&this->base_tex_coordinates);
	this->add_var(&this->mask_tex_coordinates);
}

void AlphamaskMaterial::set_positions(mesh_t positions) {
	this->position.set(positions);
}


}} // openage::renderer
