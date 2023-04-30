// Copyright 2016-2023 the openage authors. See copying.md for legal info.

#include "tile.h"

#include "../terrain/terrain.h"
#include "coordmanager.h"

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


[[deprecated]] tile3 tile::to_tile3(const Terrain & /*terrain*/, tile_t altitude) const {
	// TODO: once terrain elevations have been implemented,
	//       query the terrain elevation at {ne, se}.
	tile_t elevation = 0;

	return tile3{this->ne, this->se, elevation + altitude};
}


[[deprecated]] phys3 tile::to_phys3(const Terrain &terrain, tile_t altitude) const {
	return this->to_tile3(terrain, altitude).to_phys3();
}


[[deprecated]] camgame tile::to_camgame(const CoordManager &mgr,
                                        const Terrain &terrain,
                                        tile_t altitude) const {
	return this->to_phys3(terrain, altitude).to_camgame(mgr);
}


[[deprecated]] viewport tile::to_viewport(const CoordManager &mgr,
                                          const Terrain &terrain,
                                          tile_t altitude) const {
	return this->to_camgame(mgr, terrain, altitude).to_viewport(mgr);
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


} // namespace openage::coord
