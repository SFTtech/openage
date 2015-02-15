// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#include <algorithm>
#include <cmath>

#include "terrain_object.h"

#include "terrain.h"
#include "terrain_chunk.h"
#include "terrain_outline.h"
#include "../engine.h"
#include "../log.h"
#include "../texture.h"
#include "../coord/tile.h"
#include "../coord/tile3.h"
#include "../coord/phys3.h"
#include "../coord/camgame.h"
#include "../unit/unit.h"
#include "../util/error.h"

namespace openage {

coord::phys_t distance(const coord::phys3 &a, const coord::phys3 &b) {
	coord::phys_t dx = a.ne - b.ne;
	coord::phys_t dy = a.se - b.se;
	return std::hypot(dx, dy);
}

coord::phys3_delta normalize(const coord::phys3_delta &a, const coord::phys_t &length) {
	coord::phys3_delta result = (a * length) / std::hypot(a.ne, a.se);
	return result;
}

TerrainObject::TerrainObject(bool collisions)
	:
	unit{nullptr},
	check_collisions{collisions},
	passable{[](const coord::phys3 &) -> bool {return true;}},
	draw{[]() {}},
	placed{false},
	terrain{nullptr},
	occupied_chunk_count{0},
	parent{nullptr} {
}

TerrainObject::~TerrainObject() {
	// remove all connections from terrain
	this->remove();
	for (auto c : children) {
		c->remove();
		delete c;
	}
}

void TerrainObject::draw_outline() const {
	this->outline_texture->draw(this->pos.draw.to_camgame());
}

bool TerrainObject::place(Terrain *terrain, coord::phys3 &position) {
	if (this->placed) {
		throw util::Error("this object has already been placed.");
	}

	// use passiblity test
	if (not this->passable(position)) {
		return false;
	}

	this->place_unchecked(terrain, position);
	return true;
}

bool TerrainObject::move(coord::phys3 &position) {
	if (not this->placed) {
		return false;
	}

	// todo should do outside of this function
	bool can_move = this->passable(position);
	if (can_move) {
		this->remove();
		this->place_unchecked(terrain, position);
	}
	return can_move;
}

void TerrainObject::remove() {
	if (this->occupied_chunk_count == 0 or not this->placed) {
		return;
	}

	for (coord::tile temp_pos : tile_list(this->pos)) {
		TerrainChunk *chunk = terrain->get_chunk(temp_pos);

		if (chunk == nullptr) {
			continue;
		}

		int tile_pos = chunk->tile_position_neigh(temp_pos);

		auto &v = chunk->get_data(tile_pos)->obj;
		v.erase(std::remove(v.begin(), v.end(), this), v.end());
	}

	this->occupied_chunk_count = 0;
	this->placed = false;
}

void TerrainObject::set_ground(int id, int additional) {
	if (not this->placed) {
		throw util::Error("setting ground for object that is not placed yet.");
	}

	coord::tile temp_pos = this->pos.start;
	temp_pos.ne -= additional;
	temp_pos.se -= additional;
	while (temp_pos.ne < this->pos.end.ne + additional) {
		while (temp_pos.se < this->pos.end.se + additional) {
			TerrainChunk *chunk = terrain->get_chunk(temp_pos);

			if (chunk == nullptr) {
				continue;
			}

			size_t tile_pos = chunk->tile_position_neigh(temp_pos);
			chunk->get_data(tile_pos)->terrain_id = id;
			temp_pos.se++;
		}
		temp_pos.se = this->pos.start.se - additional;
		temp_pos.ne++;
	}
}

void TerrainObject::annex(TerrainObject *other) {
	this->children.push_back(other);
	other->parent = this;
	other->unit = this->unit;
}

const TerrainObject *TerrainObject::get_parent() const {
	return this->parent;
}

bool TerrainObject::operator <(const TerrainObject &other) {
	if (this == &other) {
		return false;
	}

	auto this_ne    = this->pos.draw.ne;
	auto this_se    = this->pos.draw.se;
	auto other_ne   = other.pos.draw.ne;
	auto other_se   = other.pos.draw.se;

	auto this_ypos  = this_ne  - this_se;
	auto other_ypos = other_ne - other_se;

	if (this_ypos < other_ypos) {
		return false;
	}
	else if (this_ypos == other_ypos) {
		if (this_ne > other_ne) {
			return false;
		}
		else if (this_ne == other_ne) {
			return this_se > other_se;
		}
	}

	return true;
}

void TerrainObject::place_unchecked(Terrain *terrain, coord::phys3 &position) {
	// storing the position:
	this->pos = get_range(position);
	this->terrain = terrain;
	this->occupied_chunk_count = 0;

	bool chunk_known = false;


	// set pointers to this object on each terrain tile
	// where the building will stand and block the ground

	for (coord::tile temp_pos : tile_list(this->pos)) {
		TerrainChunk *chunk = terrain->get_chunk(temp_pos);

		if (chunk == nullptr) {
			continue;
		}

		for (int c = 0; c < this->occupied_chunk_count; c++) {
			if (this->occupied_chunk[c] == chunk) {
				chunk_known = true;
			}
		}

		if (not chunk_known) {
			this->occupied_chunk[this->occupied_chunk_count] = chunk;
			this->occupied_chunk_count += 1;
		} else {
			chunk_known = false;
		}

		int tile_pos = chunk->tile_position_neigh(temp_pos);
		chunk->get_data(tile_pos)->obj.push_back(this);
	}

	this->placed = true;
}

SquareObject::SquareObject(coord::tile_delta foundation_size, bool collisions)
	:
	SquareObject(foundation_size, square_outline(foundation_size), collisions) {
}

SquareObject::SquareObject(coord::tile_delta foundation_size, std::shared_ptr<Texture> out_tex, bool collisions)
	:
	TerrainObject(collisions),
	size(foundation_size) {
	this->outline_texture = out_tex;
}

SquareObject::~SquareObject() {}

tile_range SquareObject::get_range(const coord::phys3 &pos) const {
	tile_range result;
	result.start = pos.to_tile3().to_tile();
	result.end   = result.start + this->size;

	// TODO temporary hacky solution until openage::coord has been properly fixed.
	coord::phys2 draw_pos = result.start.to_phys2();

	draw_pos.ne += ((this->size.ne - 1) * coord::settings::phys_per_tile) / 2;
	draw_pos.se += ((this->size.se - 1) * coord::settings::phys_per_tile) / 2;

	result.draw  = draw_pos.to_phys3();

	return result;
}

coord::phys_t SquareObject::from_edge(const coord::phys3 &point) const {
	// clamp between start and end
	coord::phys3 start_phys = this->pos.start.to_phys2().to_phys3() - phys_half_tile;
	coord::phys3 end_phys = this->pos.end.to_phys2().to_phys3() - phys_half_tile;
	coord::phys_t cx = std::max(start_phys.ne, std::min(end_phys.ne, point.ne));
	coord::phys_t cy = std::max(start_phys.se, std::min(end_phys.se, point.se));

	// distance to clamped point
	coord::phys_t dx = point.ne - cx;
	coord::phys_t dy = point.se - cy;
	return std::hypot(dx, dy);
}

coord::phys3 SquareObject::on_edge(const coord::phys3 &angle, coord::phys_t) const {
	// clamp between start and end
	coord::phys3 start_phys = this->pos.start.to_phys2().to_phys3() - phys_half_tile;
	coord::phys3 end_phys = this->pos.end.to_phys2().to_phys3() - phys_half_tile;
	coord::phys_t cx = std::max(start_phys.ne, std::min(end_phys.ne, angle.ne));
	coord::phys_t cy = std::max(start_phys.se, std::min(end_phys.se, angle.se));

	// todo use extra distance
	return coord::phys3{cx, cy, 0};
}

bool SquareObject::contains(const coord::phys3 &other) const {
	coord::tile other_tile = other.to_tile3().to_tile();

	for (coord::tile check_pos : tile_list(this->pos)) {
		if (check_pos == other_tile) {
			return true;
		}
	}
	return false;
}

bool SquareObject::intersects(const TerrainObject *other, const coord::phys3 &position) const {
	if (const SquareObject *sq = dynamic_cast<const SquareObject *>(other)) {
		tile_range rng = this->get_range(position);
		return this->pos.end.ne < rng.start.ne
		       || rng.end.ne < sq->pos.start.ne
		       || rng.end.se < sq->pos.start.se
		       || rng.end.se < sq->pos.start.se;
	}
	else if (const RadialObject *rad = dynamic_cast<const RadialObject *>(other)) {
		// clamp between start and end
		tile_range rng = this->get_range(position);
		coord::phys3 start_phys = rng.start.to_phys2().to_phys3() - phys_half_tile;
		coord::phys3 end_phys = rng.end.to_phys2().to_phys3() - phys_half_tile;
		coord::phys_t cx = std::max(start_phys.ne, std::min(end_phys.ne, rad->pos.draw.ne));
		coord::phys_t cy = std::max(start_phys.se, std::min(end_phys.se, rad->pos.draw.se));

		// distance to square object base
		coord::phys_t dx = rad->pos.draw.ne - cx;
		coord::phys_t dy = rad->pos.draw.se - cy;
		return std::hypot(dx, dy) < rad->phys_radius;
	}
	return false;
}

coord::phys_t SquareObject::min_axis() const {
	return std::min( this->size.ne, this->size.se ) * coord::settings::phys_per_tile;
}

RadialObject::RadialObject(float rad, bool collisions)
	:
	RadialObject(rad, radial_outline(rad), collisions) {
}

RadialObject::RadialObject(float rad, std::shared_ptr<Texture> out_tex, bool collisions)
	:
	TerrainObject(collisions),
	phys_radius(coord::settings::phys_per_tile * rad) {
	this->outline_texture = out_tex;
}

RadialObject::~RadialObject() {}

tile_range RadialObject::get_range(const coord::phys3 &pos) const {
	tile_range result;

	// create bounds
	coord::phys3 p_start = pos, p_end = pos;
	p_start.ne -= this->phys_radius;
	p_start.se -= this->phys_radius;
	p_end.ne += this->phys_radius;
	p_end.se += this->phys_radius;

	// set result
	result.start   = p_start.to_tile3().to_tile();
	result.end     = p_end.to_tile3().to_tile() + coord::tile_delta{ 1, 1 };
	result.draw = pos;
	return result;
}

coord::phys_t RadialObject::from_edge(const coord::phys3 &point) const {
	return std::max(distance(point, this->pos.draw) - this->phys_radius, (coord::phys_t) 0); 
}

coord::phys3 RadialObject::on_edge(const coord::phys3 &angle, coord::phys_t extra) const {
	coord::phys3_delta d = normalize(angle - this->pos.draw, this->phys_radius + extra);
	return this->pos.draw + d;
}

bool RadialObject::contains(const coord::phys3 &other) const {
	return distance(this->pos.draw, other) < this->phys_radius;
}

bool RadialObject::intersects(const TerrainObject *other, const coord::phys3 &position) const {
	if (const SquareObject *sq = dynamic_cast<const SquareObject *>(other)) {
		return sq->from_edge(position) < this->phys_radius;
	}
	else if (const RadialObject *rad = dynamic_cast<const RadialObject *>(other)) {
		return distance(position, rad->pos.draw) < this->phys_radius + rad->phys_radius;
	}
	return false;
}

coord::phys_t RadialObject::min_axis() const {
	return this->phys_radius * 2;
}

std::vector<coord::tile> tile_list(const tile_range &rng) {
	std::vector<coord::tile> tiles;

	coord::tile check_pos = rng.start;
	while (check_pos.ne < rng.end.ne) {
		while (check_pos.se < rng.end.se) {
			tiles.push_back(check_pos);
			check_pos.se += 1;
		}
		check_pos.se = rng.start.se;
		check_pos.ne += 1;
	}

	// a case when the objects radius is zero
	if (tiles.empty()) {
		tiles.push_back(rng.start);
	}
	return tiles;
}

} //namespace openage
