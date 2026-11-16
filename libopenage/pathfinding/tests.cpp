// Copyright 2015-2024 the openage authors. See copying.md for legal info.

#include <algorithm>
#include <deque>
#include <string>
#include <vector>

#include "log/log.h"
#include "testing/testing.h"

#include "coord/chunk.h"
#include "coord/tile.h"
#include "event/event_loop.h"
#include "pathfinding/cost_field.h"
#include "pathfinding/definitions.h"
#include "pathfinding/flow_field.h"
#include "pathfinding/grid.h"
#include "pathfinding/integration_field.h"
#include "pathfinding/integrator.h"
#include "pathfinding/path.h"
#include "pathfinding/pathfinder.h"
#include "pathfinding/portal.h"
#include "pathfinding/sector.h"
#include "pathfinding/types.h"
#include "time/time.h"


namespace openage {
namespace path {
namespace tests {

namespace {

/**
 * Map of cells for creating test grids; '#' marks impassable cells.
 */
using test_map_t = std::vector<std::string>;

/**
 * Map of 3x2 sectors with a side length of 4 cells.
 *
 * - sector 0 is walled off from sector 3
 * - sector 1 is split into two regions that are only connected through sector 4
 * - sector 2 is connected to sector 1 by a single cell
 * - cell (11, 7) is enclosed by walls
 */
const test_map_t REGIONS_MAP = {
	".....#..#...",
	".....#..#...",
	".....#......",
	"####.#..#...",
	"............",
	"............",
	"..........##",
	"..........#.",
};

/**
 * Map of 2x1 sectors with a side length of 4 cells.
 *
 * In both sectors, the two portals are only connected by a winding corridor,
 * so connecting them requires moving in all four directions.
 */
const test_map_t CORRIDOR_MAP = {
	"........",
	"...####.",
	"...#....",
	".....###",
};

constexpr size_t TEST_SECTOR_SIZE = 4;

bool is_test_map_passable(const test_map_t &map, coord::tile_t x, coord::tile_t y) {
	return map.at(y).at(x) != '#';
}

/**
 * Create a grid from a test map with initialized portals.
 */
std::shared_ptr<Grid> create_test_grid(const std::shared_ptr<event::EventLoop> &loop,
                                       const test_map_t &map) {
	util::Vector2s size{map.at(0).size() / TEST_SECTOR_SIZE, map.size() / TEST_SECTOR_SIZE};
	auto grid = std::make_shared<Grid>(loop, 0, size, TEST_SECTOR_SIZE);

	for (const auto &sector : grid->get_sectors()) {
		auto origin = sector->get_position().to_tile(TEST_SECTOR_SIZE);

		std::vector<cost_t> costs;
		costs.reserve(TEST_SECTOR_SIZE * TEST_SECTOR_SIZE);
		for (coord::tile_t y = 0; y < static_cast<coord::tile_t>(TEST_SECTOR_SIZE); ++y) {
			for (coord::tile_t x = 0; x < static_cast<coord::tile_t>(TEST_SECTOR_SIZE); ++x) {
				bool passable = is_test_map_passable(map, origin.ne + x, origin.se + y);
				costs.push_back(passable ? COST_MIN : COST_IMPASSABLE);
			}
		}
		sector->get_cost_field()->set_costs(std::move(costs), time::TIME_ZERO);
	}

	grid->init_portals();
	grid->init_portal_nodes();

	return grid;
}

} // namespace

void flow_field() {
	auto loop = std::make_shared<event::EventLoop>();

	// Create initial cost grid
	auto cost_field = std::make_shared<CostField>(loop, 3);

	// | 1 | 1 | 1 |
	// | 1 | X | 1 |
	// | 1 | 1 | 1 |
	const time::time_t time = time::TIME_ZERO;
	cost_field->set_costs({1, 1, 1, 1, 255, 1, 1, 1, 1}, time);

	// Test the different field types
	{
		auto integration_field = std::make_shared<IntegrationField>(3);
		integration_field->integrate_cost(cost_field, coord::tile_delta{2, 2});
		auto &int_cells = integration_field->get_cells();

		// The integration field should look like:
		// | 4 | 3 | 2 |
		// | 3 | X | 1 |
		// | 2 | 1 | 0 |
		auto int_expected = std::vector<integrated_cost_t>{
			4,
			3,
			2,
			3,
			65535,
			1,
			2,
			1,
			0,
		};

		// The flow field for targeting (2, 2) hould look like this:
		// | E  | SE | S |
		// | SE | X  | S |
		// | E  | E  | N |
		auto ff_expected = std::vector<flow_t>{
			FLOW_PATHABLE_MASK | static_cast<uint8_t>(flow_dir_t::EAST),
			FLOW_PATHABLE_MASK | static_cast<uint8_t>(flow_dir_t::SOUTH_EAST),
			FLOW_PATHABLE_MASK | static_cast<uint8_t>(flow_dir_t::SOUTH),
			FLOW_PATHABLE_MASK | static_cast<uint8_t>(flow_dir_t::SOUTH_EAST),
			0,
			FLOW_PATHABLE_MASK | static_cast<uint8_t>(flow_dir_t::SOUTH),
			FLOW_PATHABLE_MASK | static_cast<uint8_t>(flow_dir_t::EAST),
			FLOW_PATHABLE_MASK | static_cast<uint8_t>(flow_dir_t::EAST),
			FLOW_TARGET_MASK | FLOW_PATHABLE_MASK | static_cast<uint8_t>(flow_dir_t::NORTH),
		};

		// Compare the integration field cells with the expected values
		for (size_t i = 0; i < int_cells.size(); i++) {
			TESTEQUALS(int_cells[i].cost, int_expected[i]);
		}

		// Build the flow field
		auto flow_field = std::make_shared<FlowField>(3);
		flow_field->build(integration_field);
		auto ff_cells = flow_field->get_cells();

		// Compare the flow field cells with the expected values
		for (size_t i = 0; i < ff_cells.size(); i++) {
			TESTEQUALS(ff_cells[i], ff_expected[i]);
		}
	}

	// Integrator test
	{
		// Integrator for managing the flow field
		auto integrator = std::make_shared<Integrator>();

		// Build the flow field
		auto flow_field = integrator->get(cost_field, coord::tile_delta{2, 2}).second;
		auto &ff_cells = flow_field->get_cells();

		// The flow field for targeting (2, 2) hould look like this:
		// | E  | SE | S |
		// | SE | X  | S |
		// | E  | E  | N |
		auto ff_expected = std::vector<flow_t>{
			FLOW_PATHABLE_MASK | static_cast<uint8_t>(flow_dir_t::EAST),
			FLOW_PATHABLE_MASK | static_cast<uint8_t>(flow_dir_t::SOUTH_EAST),
			FLOW_LOS_MASK | FLOW_PATHABLE_MASK | static_cast<uint8_t>(flow_dir_t::SOUTH),
			FLOW_PATHABLE_MASK | static_cast<uint8_t>(flow_dir_t::SOUTH_EAST),
			0,
			FLOW_LOS_MASK | FLOW_PATHABLE_MASK | static_cast<uint8_t>(flow_dir_t::SOUTH),
			FLOW_LOS_MASK | FLOW_PATHABLE_MASK | static_cast<uint8_t>(flow_dir_t::EAST),
			FLOW_LOS_MASK | FLOW_PATHABLE_MASK | static_cast<uint8_t>(flow_dir_t::EAST),
			FLOW_LOS_MASK | FLOW_PATHABLE_MASK | FLOW_TARGET_MASK,
		};

		// Compare the flow field cells with the expected values
		for (size_t i = 0; i < ff_cells.size(); i++) {
			TESTEQUALS(ff_cells[i], ff_expected[i]);
		}
	}
}

void grid_from_sectors() {
	auto loop = std::make_shared<event::EventLoop>();

	std::vector<std::shared_ptr<Sector>> sectors;
	for (size_t y = 0; y < 2; y++) {
		for (size_t x = 0; x < 3; x++) {
			sectors.push_back(std::make_shared<Sector>(loop, sectors.size(), coord::chunk(x, y), 5));
		}
	}

	Grid grid{0, util::Vector2s{3, 2}, std::move(sectors)};
	TESTEQUALS(grid.get_sector_size(), 5);
	TESTEQUALS(grid.get_sectors().size(), 6);
}


void cost_history() {
	auto loop = std::make_shared<event::EventLoop>();

	// | 0 | 1 |
	// | 2 | 3 |
	CostField cost_field{loop, 2};
	const auto &history = cost_field.get_cost_history();
	TESTEQUALS(history.size(), 4);

	// the latest recorded costs must always equal the current costs
	auto history_is_current = [&]() {
		return history.get(time::TIME_MAX) == cost_field.get_costs();
	};

	// before any change, all cells have the initial cost at all times
	TESTEQUALS(history.at(time::TIME_MIN, 1), COST_MIN);
	TESTEQUALS(history_is_current(), true);

	// queries before, at and after a change
	cost_field.set_cost(1, 10, 5);
	TESTEQUALS(cost_field.get_cost(1), 10);
	TESTEQUALS(history.at(4, 1), COST_MIN);
	TESTEQUALS(history.at(5, 1), 10);
	TESTEQUALS(history.at(6, 1), 10);
	TESTEQUALS(history_is_current(), true);

	// all set_cost overloads record their changes
	cost_field.set_cost(0, 1, 20, 6);
	cost_field.set_cost(coord::tile_delta{1, 1}, 30, 6);
	TESTEQUALS(history.at(5, 2), COST_MIN);
	TESTEQUALS(history.at(6, 2), 20);
	TESTEQUALS(history.at(5, 3), COST_MIN);
	TESTEQUALS(history.at(6, 3), 30);
	TESTEQUALS(history_is_current(), true);

	// set_costs records all cells
	cost_field.set_costs({5, 6, 7, 8}, 8);
	TESTEQUALS(history.get(7) == std::vector<cost_t>({COST_MIN, 10, 20, 30}), true);
	TESTEQUALS(history.get(8) == std::vector<cost_t>({5, 6, 7, 8}), true);
	TESTEQUALS(history_is_current(), true);

	// changes at equal times: the last one wins
	cost_field.set_cost(3, 40, 9);
	cost_field.set_cost(3, 50, 9);
	TESTEQUALS(history.at(9, 3), 50);
	TESTEQUALS(history.at(8, 3), 8);
	TESTEQUALS(history_is_current(), true);

	// a change at an earlier time erases the later recorded changes of that cell
	cost_field.set_cost(1, COST_IMPASSABLE, 3);
	TESTEQUALS(history.at(2, 1), COST_MIN);
	TESTEQUALS(history.at(3, 1), COST_IMPASSABLE);
	TESTEQUALS(history.at(8, 1), COST_IMPASSABLE);
	TESTEQUALS(history.at(8, 0), 5);
	TESTEQUALS(history_is_current(), true);
}

namespace {

/**
 * Expected portal of a sector; entries are relative to the sector.
 */
struct expected_portal {
	sector_id_t sector;
	portal_id_t id;
	sector_id_t exit_sector;
	coord::tile_delta entry_start;
	coord::tile_delta entry_end;
	std::vector<portal_id_t> connected;
};

void check_portals(const test_map_t &map, const std::vector<expected_portal> &expected) {
	auto loop = std::make_shared<event::EventLoop>();
	auto grid = create_test_grid(loop, map);

	size_t portal_count = 0;
	for (const auto &sector : grid->get_sectors()) {
		portal_count += sector->get_portals().size();
	}
	TESTEQUALS(portal_count, expected.size());

	for (const auto &exp : expected) {
		const auto &portals = grid->get_sector(exp.sector)->get_portals();
		auto portal = std::ranges::find_if(portals, [&](const auto &p) {
			return p->get_id() == exp.id;
		});
		if (portal == portals.end()) {
			TESTFAILMSG("portal " << exp.id << " missing in sector " << exp.sector);
		}

		TESTEQUALS((*portal)->get_exit_sector(exp.sector), exp.exit_sector);
		TESTEQUALS((*portal)->get_entry_start(exp.sector), exp.entry_start);
		TESTEQUALS((*portal)->get_entry_end(exp.sector), exp.entry_end);

		std::vector<portal_id_t> connected;
		for (const auto &other : (*portal)->get_connected(exp.sector)) {
			connected.push_back(other->get_id());
		}
		std::ranges::sort(connected);
		if (connected != exp.connected) {
			TESTFAILMSG("portal " << exp.id << " in sector " << exp.sector
			                      << " has wrong connected portals");
		}
	}
}

/**
 * Check the paths from each start to every cell of a test map against
 * the connected regions of its passable cells.
 */
void check_paths(const test_map_t &map, const std::vector<coord::tile> &starts) {
	auto loop = std::make_shared<event::EventLoop>();
	auto grid = create_test_grid(loop, map);
	Pathfinder pathfinder{};
	pathfinder.add_grid(grid);

	const auto width = static_cast<coord::tile_t>(map.at(0).size());
	const auto height = static_cast<coord::tile_t>(map.size());

	// reference solution: label the connected regions of passable cells
	std::vector<int> region(width * height, -1);
	int region_count = 0;
	for (coord::tile_t i = 0; i < width * height; ++i) {
		if (region[i] != -1 or not is_test_map_passable(map, i % width, i / width)) {
			continue;
		}

		std::deque<coord::tile_t> open_list{i};
		region[i] = region_count;
		while (not open_list.empty()) {
			auto current = open_list.front();
			open_list.pop_front();
			coord::tile_t x = current % width;
			coord::tile_t y = current / width;
			for (auto [nx, ny] : {std::pair{x, y - 1}, {x - 1, y}, {x, y + 1}, {x + 1, y}}) {
				if (nx < 0 or ny < 0 or nx >= width or ny >= height) {
					continue;
				}
				auto neighbor = nx + ny * width;
				if (region[neighbor] == -1 and is_test_map_passable(map, nx, ny)) {
					region[neighbor] = region_count;
					open_list.push_back(neighbor);
				}
			}
		}
		++region_count;
	}
	auto region_of = [&](const coord::tile &tile) {
		return region.at(tile.ne + tile.se * width);
	};

	for (const auto &start : starts) {
		if (not is_test_map_passable(map, start.ne, start.se)) {
			TESTFAILMSG("test start " << start << " is impassable");
		}

		for (coord::tile_t y = 0; y < height; ++y) {
			for (coord::tile_t x = 0; x < width; ++x) {
				coord::tile target{x, y};
				auto path = pathfinder.get_path(PathRequest{grid->get_id(), start, target, time::TIME_ZERO});

				bool reachable = is_test_map_passable(map, x, y) and region_of(target) == region_of(start);
				if (not reachable) {
					if (path.status != PathResult::NOT_FOUND or not path.waypoints.empty()) {
						TESTFAILMSG("path from " << start << " to unreachable " << target << " was found");
					}
					continue;
				}

				if (path.status != PathResult::FOUND or path.waypoints.empty()) {
					TESTFAILMSG("path from " << start << " to " << target << " was not found");
				}
				if (path.waypoints.front() != start or path.waypoints.back() != target) {
					TESTFAILMSG("path from " << start << " to " << target
					                         << " does not begin at the start and end at the target");
				}
				for (const auto &waypoint : path.waypoints) {
					if (waypoint.ne < 0 or waypoint.se < 0 or waypoint.ne >= width or waypoint.se >= height
					    or not is_test_map_passable(map, waypoint.ne, waypoint.se)
					    or region_of(waypoint) != region_of(start)) {
						TESTFAILMSG("path from " << start << " to " << target
						                         << " has invalid waypoint " << waypoint);
					}
				}
			}
		}

		for (const auto &target : {coord::tile{-1, 0}, coord::tile{0, -1}, coord::tile{width, 0}, coord::tile{0, height}}) {
			auto path = pathfinder.get_path(PathRequest{grid->get_id(), start, target, time::TIME_ZERO});
			if (path.status != PathResult::OUT_OF_BOUNDS) {
				TESTFAILMSG("path from " << start << " to " << target << " is not out of bounds");
			}
		}
	}
}

} // namespace

void portals() {
	check_portals(REGIONS_MAP,
	              {
					  {0, 0, 1, {3, 0}, {3, 2}, {}},
					  {1, 0, 0, {0, 0}, {0, 2}, {2}},
					  {1, 1, 2, {3, 2}, {3, 2}, {3}},
					  {1, 2, 4, {0, 3}, {0, 3}, {0}},
					  {1, 3, 4, {2, 3}, {3, 3}, {1}},
					  {2, 1, 1, {0, 2}, {0, 2}, {4}},
					  {2, 4, 5, {1, 3}, {3, 3}, {1}},
					  {3, 5, 4, {3, 0}, {3, 3}, {}},
					  {4, 2, 1, {0, 0}, {0, 0}, {3, 5, 6}},
					  {4, 3, 1, {2, 0}, {3, 0}, {2, 5, 6}},
					  {4, 5, 3, {0, 0}, {0, 3}, {2, 3, 6}},
					  {4, 6, 5, {3, 0}, {3, 3}, {2, 3, 5}},
					  {5, 4, 2, {1, 0}, {3, 0}, {6}},
					  {5, 6, 4, {0, 0}, {0, 3}, {4}},
				  });

	check_portals(CORRIDOR_MAP,
	              {
					  {0, 0, 1, {3, 0}, {3, 0}, {1}},
					  {0, 1, 1, {3, 3}, {3, 3}, {0}},
					  {1, 0, 0, {0, 0}, {0, 0}, {1}},
					  {1, 1, 0, {0, 3}, {0, 3}, {0}},
				  });
}

void pathfinder() {
	// (4, 0) and (7, 0) are in the same sector but only connected through sector 4
	check_paths(REGIONS_MAP, {{0, 0}, {4, 0}, {7, 0}, {9, 1}, {0, 7}, {6, 6}});

	check_paths(CORRIDOR_MAP, {{0, 0}, {7, 0}, {5, 2}, {0, 3}});
}

} // namespace tests
} // namespace path
} // namespace openage
