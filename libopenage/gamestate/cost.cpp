// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "cost.h"
#include "player.h"


namespace openage {

ResourceCost::ResourceCost()
	:
	type{cost_type::constant},
	resources{} {}

ResourceCost::ResourceCost(const ResourceBundle& resources)
	:
	type{cost_type::constant},
	resources{} {
	this->resources.set(resources);
}

ResourceCost::ResourceCost(cost_type type, const ResourceBundle& multiplier)
	:
	type{type},
	resources{} {
	this->resources.set(multiplier);
}

ResourceCost::~ResourceCost() {
}

void ResourceCost::set(cost_type type, const ResourceBundle& resources) {
	this->type = type;
	this->resources.set(resources);
}

const ResourceBundle ResourceCost::get(const Player& player) const {
	if (type == cost_type::constant) {
		return resources;
	}

	// calculate dynamic cost
	ResourceBundle resources = this->resources.clone();
	if (type == cost_type::workforce) {
		resources *= player.get_workforce_count();
	}
	return resources;
}

} // openage
