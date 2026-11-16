// Copyright 2015-2024 the openage authors. See copying.md for legal info.

#include "log/log.h"
#include "testing/testing.h"

#include "coord/tile.h"
#include "event/event_loop.h"
#include "pathfinding/cost_field.h"
#include "pathfinding/definitions.h"
#include "pathfinding/flow_field.h"
#include "pathfinding/grid.h"
#include "pathfinding/integration_field.h"
#include "pathfinding/integrator.h"
#include "pathfinding/sector.h"
#include "pathfinding/types.h"
#include "time/time.h"


namespace openage {
namespace path {
namespace tests {

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

} // namespace tests
} // namespace path
} // namespace openage
