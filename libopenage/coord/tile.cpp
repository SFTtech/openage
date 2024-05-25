// Copyright 2016-2024 the openage authors. See copying.md for legal info.

#include "tile.h"

#include "chunk.h"
#include "phys.h"


namespace openage::coord {

phys2_delta tile_delta::to_phys2() const {
	return phys2_delta{phys2::elem_t::from_int(this->ne),
	                   phys2::elem_t::from_int(this->se)};
}

phys3_delta tile_delta::to_phys3(tile_t up) const {
	return phys3_delta{phys3::elem_t::from_int(this->ne),
	                   phys3::elem_t::from_int(this->se),
	                   phys3::elem_t::from_int(up)};
}

tile3 tile::to_tile3(tile_t up) const {
	return tile3{this->ne, this->se, up};
}

phys2 tile::to_phys2() const {
	return phys2{phys3::elem_t::from_int(this->ne),
	             phys3::elem_t::from_int(this->se)};
}

phys3 tile::to_phys3(tile_t up) const {
	return this->to_tile3(up).to_phys3();
}

phys2 tile::to_phys2_center() const {
	return phys2{phys3::elem_t::from_int(this->ne) + 0.5,
	             phys3::elem_t::from_int(this->se) + 0.5};
}

phys3 tile::to_phys3_center(tile_t up) const {
	return phys3{phys3::elem_t::from_int(this->ne) + 0.5,
	             phys3::elem_t::from_int(this->se) + 0.5,
	             phys3::elem_t::from_int(up)};
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

tile_delta tile3_delta::to_tile() const {
	return tile_delta{this->ne, this->se};
}

phys3_delta tile3_delta::to_phys3() const {
	return phys3_delta{phys3::elem_t::from_int(this->ne),
	                   phys3::elem_t::from_int(this->se),
	                   phys3::elem_t::from_int(up)};
}

tile tile3::to_tile() const {
	return tile{this->ne, this->se};
}

phys2 tile3::to_phys2() const {
	return this->to_tile().to_phys2();
}

phys3 tile3::to_phys3() const {
	return phys3{phys3::elem_t::from_int(this->ne),
	             phys3::elem_t::from_int(this->se),
	             phys3::elem_t::from_int(this->up)};
}

phys2 tile3::to_phys2_center() const {
	return phys2{phys3::elem_t::from_int(this->ne) + 0.5,
	             phys3::elem_t::from_int(this->se) + 0.5};
}

phys3 tile3::to_phys3_center() const {
	return phys3{phys3::elem_t::from_int(this->ne) + 0.5,
	             phys3::elem_t::from_int(this->se) + 0.5,
	             phys3::elem_t::from_int(this->up)};
}

} // namespace openage::coord
