// Copyright 2015-2024 the openage authors. See copying.md for legal info.

#include "log/log.h"
#include "testing/testing.h"

#include "coord/tile.h"
#include "pathfinding/cost_field.h"
#include "pathfinding/definitions.h"
#include "pathfinding/flow_field.h"
#include "pathfinding/integration_field.h"
#include "pathfinding/integrator.h"
#include "pathfinding/types.h"
#include "time/time.h"


namespace openage {
namespace path {
namespace tests {

void flow_field() {
	// Create initial cost grid
	auto cost_field = std::make_shared<CostField>(3);

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


} // namespace tests
} // namespace path
} // namespace openage
