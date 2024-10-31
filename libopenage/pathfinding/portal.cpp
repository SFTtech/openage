// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "portal.h"

#include "error/error.h"


namespace openage::path {

Portal::Portal(portal_id_t id,
               sector_id_t sector0,
               sector_id_t sector1,
               PortalDirection direction,
               const coord::tile_delta &cell_start,
               const coord::tile_delta &cell_end) :
	id{id},
	sector0{sector0},
	sector1{sector1},
	sector0_exits{},
	sector1_exits{},
	direction{direction},
	cell_start{cell_start},
	cell_end{cell_end} {
}

portal_id_t Portal::get_id() const {
	return this->id;
}

const std::vector<std::shared_ptr<Portal>> &Portal::get_connected(sector_id_t sector) const {
	ENSURE(sector == this->sector0 || sector == this->sector1, "Portal does not connect to sector");

	if (sector == this->sector0) {
		return this->sector0_exits;
	}
	return this->sector1_exits;
}

const std::vector<std::shared_ptr<Portal>> &Portal::get_exits(sector_id_t entry_sector) const {
	ENSURE(entry_sector == this->sector0 || entry_sector == this->sector1, "Invalid entry sector");

	if (entry_sector == this->sector0) {
		return this->sector1_exits;
	}
	return this->sector0_exits;
}

void Portal::set_exits(sector_id_t sector, const std::vector<std::shared_ptr<Portal>> &exits) {
	ENSURE(sector == this->sector0 || sector == this->sector1, "Portal does not connect to sector");

	if (sector == this->sector0) {
		this->sector0_exits = exits;
	}
	else {
		this->sector1_exits = exits;
	}
}

sector_id_t Portal::get_exit_sector(sector_id_t entry_sector) const {
	ENSURE(entry_sector == this->sector0 || entry_sector == this->sector1, "Invalid entry sector");

	if (entry_sector == this->sector0) {
		return this->sector1;
	}
	return this->sector0;
}

const coord::tile_delta Portal::get_entry_start(sector_id_t entry_sector) const {
	ENSURE(entry_sector == this->sector0 || entry_sector == this->sector1, "Invalid entry sector");

	if (entry_sector == this->sector0) {
		return this->get_sector0_start();
	}

	return this->get_sector1_start();
}

const coord::tile_delta Portal::get_entry_center(sector_id_t entry_sector) const {
	ENSURE(entry_sector == this->sector0 || entry_sector == this->sector1, "Invalid entry sector");

	if (entry_sector == this->sector0) {
		auto start = this->get_sector0_start();
		auto end = this->get_sector0_end();
		return {(start.ne + end.ne) / 2, (start.se + end.se) / 2};
	}

	auto start = this->get_sector1_start();
	auto end = this->get_sector1_end();
	return {(start.ne + end.ne) / 2, (start.se + end.se) / 2};
}

const coord::tile_delta Portal::get_entry_end(sector_id_t entry_sector) const {
	ENSURE(entry_sector == this->sector0 || entry_sector == this->sector1, "Invalid entry sector");

	if (entry_sector == this->sector0) {
		return this->get_sector0_end();
	}

	return this->get_sector1_end();
}

const coord::tile_delta Portal::get_exit_start(sector_id_t entry_sector) const {
	ENSURE(entry_sector == this->sector0 || entry_sector == this->sector1, "Invalid entry sector");

	if (entry_sector == this->sector0) {
		return this->get_sector1_start();
	}

	return this->get_sector0_start();
}

const coord::tile_delta Portal::get_exit_center(sector_id_t entry_sector) const {
	ENSURE(entry_sector == this->sector0 || entry_sector == this->sector1, "Invalid entry sector");

	if (entry_sector == this->sector0) {
		auto start = this->get_sector1_start();
		auto end = this->get_sector1_end();
		return {(start.ne + end.ne) / 2, (start.se + end.se) / 2};
	}

	auto start = this->get_sector0_start();
	auto end = this->get_sector0_end();
	return {(start.ne + end.ne) / 2, (start.se + end.se) / 2};
}

const coord::tile_delta Portal::get_exit_end(sector_id_t entry_sector) const {
	ENSURE(entry_sector == this->sector0 || entry_sector == this->sector1, "Invalid entry sector");

	if (entry_sector == this->sector0) {
		return this->get_sector1_end();
	}

	return this->get_sector0_end();
}

PortalDirection Portal::get_direction() const {
	return this->direction;
}

const coord::tile_delta &Portal::get_sector0_start() const {
	return this->cell_start;
}

const coord::tile_delta &Portal::get_sector0_end() const {
	return this->cell_end;
}

const coord::tile_delta Portal::get_sector1_start() const {
	if (this->direction == PortalDirection::NORTH_SOUTH) {
		return {this->cell_start.ne, 0};
	}
	return {0, this->cell_start.se};
}

const coord::tile_delta Portal::get_sector1_end() const {
	if (this->direction == PortalDirection::NORTH_SOUTH) {
		return {this->cell_end.ne, 0};
	}
	return {0, this->cell_end.se};
}

} // namespace openage::path
