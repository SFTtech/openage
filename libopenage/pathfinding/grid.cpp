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
				std::make_shared<Sector>(x + y * this->size[0],
			                             coord::chunk(x, y),
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

void Grid::init_portals() {
	// Create portals between neighboring sectors.
	portal_id_t portal_id = 0;
	for (size_t y = 0; y < this->size[1]; y++) {
		for (size_t x = 0; x < this->size[0]; x++) {
			auto sector = this->get_sector(x, y);

			if (x < this->size[0] - 1) {
				auto neighbor = this->get_sector(x + 1, y);
				auto portals = sector->find_portals(neighbor, PortalDirection::EAST_WEST, portal_id);
				for (auto &portal : portals) {
					sector->add_portal(portal);
					neighbor->add_portal(portal);
				}
				portal_id += portals.size();
			}
			if (y < this->size[1] - 1) {
				auto neighbor = this->get_sector(x, y + 1);
				auto portals = sector->find_portals(neighbor, PortalDirection::NORTH_SOUTH, portal_id);
				for (auto &portal : portals) {
					sector->add_portal(portal);
					neighbor->add_portal(portal);
				}
				portal_id += portals.size();
			}
		}
	}

	// Connect mutually reachable exits of sectors.
	for (auto &sector : this->sectors) {
		sector->connect_exits();
	}
}

const nodemap_t &Grid::get_portal_map() {
	return portal_nodes;
}

void Grid::init_portal_nodes() {
	// create portal_nodes
	for (auto &sector : this->sectors) {
		for (auto &portal : sector->get_portals()) {
			if (!this->portal_nodes.contains(portal->get_id())) {
				auto portal_node = std::make_shared<PortalNode>(portal);
				portal_node->node_sector_0 = sector->get_id();
				portal_node->node_sector_1 = portal_node->portal->get_exit_sector(sector->get_id());
				this->portal_nodes[portal->get_id()] = portal_node;
			}
		}
	}

	// init portal_node exits
	for (auto &[id, node] : this->portal_nodes) {
		node->init_exits(this->portal_nodes);
	}
}

} // namespace openage::path
