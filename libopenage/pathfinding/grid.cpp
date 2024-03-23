// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "grid.h"

#include "error/error.h"

#include "pathfinding/sector.h"


namespace openage::path {

Grid::Grid(grid_id_t id, size_t width, size_t height, size_t sector_size) :
	id{id},
	width{width},
	height{height} {
	for (size_t y = 0; y < height; y++) {
		for (size_t x = 0; x < width; x++) {
			this->sectors.push_back(std::make_shared<Sector>(x + y * width, sector_size));
		}
	}
}

Grid::Grid(grid_id_t id,
           size_t width,
           size_t height,
           std::vector<std::shared_ptr<Sector>> &&sectors) :
	id{id},
	width{width},
	height{height},
	sectors{std::move(sectors)} {
	ENSURE(this->sectors.size() == width * height,
	       "Grid has size " << width << "x" << height << " (" << width * height << " sectors), "
	                        << "but only " << this->sectors.size() << " sectors were provided");
}

grid_id_t Grid::get_id() const {
	return this->id;
}

std::pair<size_t, size_t> Grid::get_size() const {
	return {this->width, this->height};
}

const std::shared_ptr<Sector> &Grid::get_sector(size_t x, size_t y) {
	return this->sectors.at(x + y * this->width);
}

const std::shared_ptr<Sector> &Grid::get_sector(sector_id_t id) const {
	return this->sectors.at(id);
}

} // namespace openage::path
