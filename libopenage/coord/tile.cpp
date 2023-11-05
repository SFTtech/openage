// Copyright 2016-2023 the openage authors. See copying.md for legal info.

#include "tile.h"

#include "../terrain/terrain.h"

namespace openage::coord {


tile3 tile::to_tile3(tile_t up) const {
	return tile3(this->ne, this->se, up);
}


phys2 tile::to_phys2() const {
	return phys2{phys3::elem_t::from_int(this->ne), phys3::elem_t::from_int(this->se)};
}


phys3 tile::to_phys3(tile_t up) const {
	return this->to_tile3(up).to_phys3();
}


chunk tile::to_chunk() const {
	return chunk{
		static_cast<chunk::elem_t>(util::div(this->ne, tiles_per_chunk)),
		static_cast<chunk::elem_t>(util::div(this->se, tiles_per_chunk))};
}


tile_delta tile::get_pos_on_chunk() const {
	return tile_delta{
		util::mod(this->ne, tiles_per_chunk),
		util::mod(this->se, tiles_per_chunk)};
}


phys2 tile3::to_phys2() const {
	return this->to_tile().to_phys2();
}


phys3 tile3::to_phys3() const {
	return phys3{
		phys3::elem_t::from_int(this->ne),
		phys3::elem_t::from_int(this->se),
		phys3::elem_t::from_int(this->up)};
}


phys2_delta tile_delta::to_phys2() const {
	return phys2_delta(this->ne, this->se);
}

phys3_delta tile_delta::to_phys3(tile_t up) const {
	return phys3_delta(this->ne, this->se, up);
}

} // namespace openage::coord
