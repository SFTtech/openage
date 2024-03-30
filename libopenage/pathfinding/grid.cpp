// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "grid.h"

#include "error/error.h"

#include "coord/chunk.h"
#include "pathfinding/cost_field.h"
#include "pathfinding/sector.h"


namespace openage::path {

Grid::Grid(grid_id_t id,
           const util::Vector2s &size,
           size_t sector_size) :
	id{id},
	size{size},
	sector_size{sector_size} {
	for (size_t y = 0; y < size[1]; y++) {
		for (size_t x = 0; x < size[0]; x++) {
			this->sectors.push_back(
				std::make_shared<Sector>(
					x + y * this->size[0],
					coord::chunk{x, y},
					sector_size));
		}
	}
}

Grid::Grid(grid_id_t id,
           const util::Vector2s &size,
           std::vector<std::shared_ptr<Sector>> &&sectors) :
	id{id},
	size{size},
	sectors{std::move(sectors)} {
	ENSURE(this->sectors.size() == size[0] * size[1],
	       "Grid has size " << size[0] << "x" << size[1] << " (" << size[0] * size[1] << " sectors), "
	                        << "but only " << this->sectors.size() << " sectors were provided");

	this->sector_size = sectors.at(0)->get_cost_field()->get_size();
}

grid_id_t Grid::get_id() const {
	return this->id;
}

const util::Vector2s &Grid::get_size() const {
	return this->size;
}

size_t Grid::get_sector_size() const {
	return this->sector_size;
}

const std::shared_ptr<Sector> &Grid::get_sector(size_t x, size_t y) {
	return this->sectors.at(x + y * this->size[0]);
}

const std::shared_ptr<Sector> &Grid::get_sector(sector_id_t id) const {
	return this->sectors.at(id);
}

const std::vector<std::shared_ptr<Sector>> &Grid::get_sectors() const {
	return this->sectors;
}

} // namespace openage::path
