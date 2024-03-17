// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "portal.h"

#include "error/error.h"


namespace openage::path {

Portal::Portal(std::shared_ptr<CostField> sector0,
               std::shared_ptr<CostField> sector1,
               std::vector<std::shared_ptr<Portal>> sector0_exits,
               std::vector<std::shared_ptr<Portal>> sector1_exits,
               PortalDirection direction,
               size_t cell_start_x,
               size_t cell_start_y,
               size_t cell_end_x,
               size_t cell_end_y) :
	sector0{sector0},
	sector1{sector1},
	sector0_exits{sector0_exits},
	sector1_exits{sector1_exits},
	direction{direction},
	cell_start_x{cell_start_x},
	cell_start_y{cell_start_y},
	cell_end_x{cell_end_x},
	cell_end_y{cell_end_y} {
}

const std::vector<std::shared_ptr<Portal>> &Portal::get_exits(const std::shared_ptr<CostField> &entry_sector) const {
	ENSURE(entry_sector == this->sector0 || entry_sector == this->sector1, "Invalid entry sector");

	if (entry_sector == this->sector0) {
		return this->sector1_exits;
	}
	return this->sector0_exits;
}

const std::shared_ptr<CostField> &Portal::get_exit_sector(const std::shared_ptr<CostField> &entry_sector) const {
	ENSURE(entry_sector == this->sector0 || entry_sector == this->sector1, "Invalid entry sector");

	if (entry_sector == this->sector0) {
		return this->sector1;
	}
	return this->sector0;
}

std::pair<size_t, size_t> Portal::get_entry_start(const std::shared_ptr<CostField> &entry_sector) const {
	ENSURE(entry_sector == this->sector0 || entry_sector == this->sector1, "Invalid entry sector");

	if (entry_sector == this->sector0) {
		return this->get_sector0_start();
	}

	return this->get_sector1_start();
}

std::pair<size_t, size_t> Portal::get_entry_end(const std::shared_ptr<CostField> &entry_sector) const {
	ENSURE(entry_sector == this->sector0 || entry_sector == this->sector1, "Invalid entry sector");

	if (entry_sector == this->sector0) {
		return this->get_sector0_end();
	}

	return this->get_sector1_end();
}

std::pair<size_t, size_t> Portal::get_exit_start(const std::shared_ptr<CostField> &entry_sector) const {
	ENSURE(entry_sector == this->sector0 || entry_sector == this->sector1, "Invalid entry sector");

	if (entry_sector == this->sector0) {
		return this->get_sector1_start();
	}

	return this->get_sector0_start();
}

std::pair<size_t, size_t> Portal::get_exit_end(const std::shared_ptr<CostField> &entry_sector) const {
	ENSURE(entry_sector == this->sector0 || entry_sector == this->sector1, "Invalid entry sector");

	if (entry_sector == this->sector0) {
		return this->get_sector1_end();
	}

	return this->get_sector0_end();
}

std::pair<size_t, size_t> Portal::get_sector0_start() const {
	return {this->cell_start_x, this->cell_start_y};
}

std::pair<size_t, size_t> Portal::get_sector0_end() const {
	return {this->cell_end_x, this->cell_end_y};
}

std::pair<size_t, size_t> Portal::get_sector1_start() const {
	return {this->cell_end_x, this->cell_end_y};
}

std::pair<size_t, size_t> Portal::get_sector1_end() const {
	return {this->cell_start_x, this->cell_start_y};
}

} // namespace openage::path
