// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "demo_1.h"

#include "pathfinding/cost_field.h"
#include "pathfinding/grid.h"
#include "pathfinding/portal.h"
#include "pathfinding/sector.h"


namespace openage::path::tests {

void path_demo_1(const util::Path &path) {
	auto grid = std::make_shared<Grid>(0, util::Vector2s{4, 3}, 10);

	// Initialize the cost field for each sector.
	for (auto sector : grid->get_sectors()) {
		auto cost_field = sector->get_cost_field();
		auto sector_cost = sectors_cost.at(sector->get_id());
		cost_field->set_costs(std::move(sector_cost));
	}

	// Initialize portals between sectors.
	auto grid_size = grid->get_size();
	auto portal_id = 0;
	for (size_t y = 0; y < grid_size[1]; y++) {
		for (size_t x = 0; x < grid_size[0]; x++) {
			auto sector = grid->get_sector(x, y);

			if (x < grid_size[0] - 1) {
				auto neighbor = grid->get_sector(x + 1, y);
				auto portals = sector->find_portals(neighbor, PortalDirection::EAST_WEST, portal_id);
				for (auto portal : portals) {
					sector->add_portal(portal);
					neighbor->add_portal(portal);
				}
				portal_id += portals.size();
			}
			if (y < grid_size[1] - 1) {
				auto neighbor = grid->get_sector(x, y + 1);
				auto portals = sector->find_portals(neighbor, PortalDirection::NORTH_SOUTH, portal_id);
				for (auto portal : portals) {
					sector->add_portal(portal);
					neighbor->add_portal(portal);
				}
				portal_id += portals.size();
			}
		}
	}

	// Connect portals inside sectors.
	for (auto sector : grid->get_sectors()) {
		sector->connect_exits();

		log::log(MSG(info) << "Sector " << sector->get_id() << " has " << sector->get_portals().size() << " portals.");
		for (auto portal : sector->get_portals()) {
			log::log(MSG(info) << "  Portal " << portal->get_id() << ":");
			log::log(MSG(info) << "    Connects sectors:  " << sector->get_id() << " to "
			                   << portal->get_exit_sector(sector->get_id()));
			log::log(MSG(info) << "    Entry start:       " << portal->get_entry_start(sector->get_id()));
			log::log(MSG(info) << "    Entry end:         " << portal->get_entry_end(sector->get_id()));
			log::log(MSG(info) << "    Connected portals: " << portal->get_connected(sector->get_id()).size());
		}
	}
}
} // namespace openage::path::tests
