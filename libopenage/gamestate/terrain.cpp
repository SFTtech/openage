// Copyright 2018-2018 the openage authors. See copying.md for legal info.

#include "terrain.h"

#include "renderer/terrain/terrain_render_entity.h"

namespace openage::gamestate {

Terrain::Terrain() :
	height_map{},
	texture_path{},
	render_entity{} {
}

void Terrain::push_to_render() {
	if (this->render_entity != nullptr) {
		this->render_entity->update();
	}
}

} // namespace openage::gamestate
