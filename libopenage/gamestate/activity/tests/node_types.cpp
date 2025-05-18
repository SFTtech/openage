// Copyright 2024-2025 the openage authors. See copying.md for legal info.

#include <memory>

#include "gamestate/activity/end_node.h"
#include "gamestate/activity/start_node.h"
#include "gamestate/activity/task_node.h"
#include "gamestate/activity/task_system_node.h"
#include "gamestate/activity/types.h"
#include "gamestate/activity/xor_event_gate.h"
#include "gamestate/activity/xor_gate.h"
#include "gamestate/activity/xor_switch_gate.h"
#include "gamestate/system/types.h"
#include "testing/testing.h"


namespace openage::gamestate::activity::tests {

/**
 * Unit tests for the activity node types.
 */
void node_types() {
	// Start node type
	{
		auto start_node = std::make_shared<StartNode>(0);

		// Check basic properties inherited from Node interface
		TESTEQUALS(start_node->get_id(), 0);
		TESTEQUALS(static_cast<int>(start_node->get_type()), static_cast<int>(node_t::START));
		TESTEQUALS(start_node->get_label(), "Start");
		TESTEQUALS(start_node->str(), "Start (id=0)");

		auto next_node = std::make_shared<EndNode>(1);
		start_node->add_output(next_node);

		// Check the next node
		TESTEQUALS(start_node->get_next(), 1);
		TESTEQUALS(start_node->next(1), next_node);

		// Check that the node throws errors for invalid output IDs
		TESTTHROWS(start_node->next(999));
	}

	// End node type
	{
		auto end_node = std::make_shared<EndNode>(0);

		// Check basic properties inherited from Node interface
		TESTEQUALS(end_node->get_id(), 0);
		// TESTEQUALS(end_node->get_type(), node_t::END);
		TESTEQUALS(end_node->get_label(), "End");
		TESTEQUALS(end_node->str(), "End (id=0)");

		// End nodes have no outputs
		TESTTHROWS(end_node->next(999));
	}

	// Task system node type
	{
		auto task_system_node = std::make_shared<TaskSystemNode>(0);

		// Check basic properties inherited from Node interface
		TESTEQUALS(task_system_node->get_id(), 0);
		// TESTEQUALS(task_system_node->get_type(), node_t::TASK_SYSTEM);
		TESTEQUALS(task_system_node->get_label(), "TaskSystem");
		TESTEQUALS(task_system_node->str(), "TaskSystem (id=0)");

		auto next_node = std::make_shared<EndNode>(1);
		task_system_node->add_output(next_node);

		// Check the next node
		TESTEQUALS(task_system_node->get_next(), 1);
		TESTEQUALS(task_system_node->next(1), next_node);

		// Check that the node throws errors for invalid output IDs
		TESTTHROWS(task_system_node->next(999));

		auto sytem_id = system::system_id_t::IDLE;
		task_system_node->set_system_id(sytem_id);

		// Check the system ID
		TESTEQUALS(static_cast<int>(task_system_node->get_system_id()), static_cast<int>(sytem_id));
	}

	// Task custom node type
	{
		auto task_custom_node = std::make_shared<TaskCustom>(0);

		// Check basic properties inherited from Node interface
		TESTEQUALS(task_custom_node->get_id(), 0);
		// TESTEQUALS(task_custom_node->get_type(), node_t::TASK_CUSTOM);
		TESTEQUALS(task_custom_node->get_label(), "TaskCustom");
		TESTEQUALS(task_custom_node->str(), "TaskCustom (id=0)");

		auto next_node = std::make_shared<EndNode>(1);
		task_custom_node->add_output(next_node);

		// Check the next node
		TESTEQUALS(task_custom_node->get_next(), 1);
		TESTEQUALS(task_custom_node->next(1), next_node);

		// Check that the node throws errors for invalid output IDs
		TESTTHROWS(task_custom_node->next(999));

		auto task_func = [](const time::time_t & /* time */,
		                    const std::shared_ptr<GameEntity> & /* entity */) {
			// Do nothing
		};
		task_custom_node->set_task_func(task_func);

		// Check the task function
		auto get_func = task_custom_node->get_task_func();
		get_func(time::time_t{0}, nullptr);
	}

	// Xor gate node type
	{
		auto xor_gate_node = std::make_shared<XorGate>(0);

		// Check basic properties inherited from Node interface
		TESTEQUALS(xor_gate_node->get_id(), 0);
		// TESTEQUALS(xor_gate_node->get_type(), node_t::XOR_GATE);
		TESTEQUALS(xor_gate_node->get_label(), "ExclusiveGateway");
		TESTEQUALS(xor_gate_node->str(), "ExclusiveGateway (id=0)");

		auto default_node = std::make_shared<EndNode>(1);
		xor_gate_node->set_default(default_node);

		// Check the default node
		TESTEQUALS(xor_gate_node->get_default(), default_node);

		auto option1_node = std::make_shared<EndNode>(2);
		xor_gate_node->add_output(option1_node,
		                          {nullptr,
		                           [](const time::time_t &time,
		                              const std::shared_ptr<GameEntity> & /* entity */,
		                              const std::shared_ptr<gamestate::GameState> & /* state */,
		                              const std::shared_ptr<nyan::Object> & /* api_object */) {
									   return time == time::TIME_ZERO;
								   }});

		auto option2_node = std::make_shared<EndNode>(3);
		xor_gate_node->add_output(option2_node,
		                          {nullptr,
		                           [](const time::time_t &time,
		                              const std::shared_ptr<GameEntity> & /* entity */,
		                              const std::shared_ptr<gamestate::GameState> & /* state */,
		                              const std::shared_ptr<nyan::Object> & /* api_object */) {
									   return time == time::TIME_MAX;
								   }});

		auto conditions = xor_gate_node->get_conditions();

		// Check the conditions
		TESTEQUALS(conditions.size(), 2);

		// Check if the conditions are set correctly
		// we don't pass GameEntity, GameState, or nyan::Object as they are not used by the condition functions
		TESTEQUALS(conditions.at(2).function(time::TIME_ZERO, nullptr, nullptr, nullptr), true);
		TESTEQUALS(conditions.at(3).function(time::TIME_ZERO, nullptr, nullptr, nullptr), false);

		TESTEQUALS(conditions.at(2).function(time::TIME_MAX, nullptr, nullptr, nullptr), false);
		TESTEQUALS(conditions.at(3).function(time::TIME_MAX, nullptr, nullptr, nullptr), true);

		// Check if next nodes return correctly
		TESTEQUALS(xor_gate_node->next(1), default_node);
		TESTEQUALS(xor_gate_node->next(2), option1_node);
		TESTEQUALS(xor_gate_node->next(3), option2_node);

		// Check that the node throws errors for invalid output IDs
		TESTTHROWS(xor_gate_node->next(999));
	}

	// Xor switch gate node type
	{
		auto xor_switch_gate_node = std::make_shared<XorSwitchGate>(0);

		// Check basic properties inherited from Node interface
		TESTEQUALS(xor_switch_gate_node->get_id(), 0);
		// TESTEQUALS(xor_switch_gate_node->get_type(), node_t::XOR_SWITCH_GATE);
		TESTEQUALS(xor_switch_gate_node->get_label(), "ExclusiveSwitchGateway");
		TESTEQUALS(xor_switch_gate_node->str(), "ExclusiveSwitchGateway (id=0)");

		auto default_node = std::make_shared<EndNode>(1);
		xor_switch_gate_node->set_default(default_node);

		// Check the default node
		TESTEQUALS(xor_switch_gate_node->get_default(), default_node);

		auto option1_node = std::make_shared<EndNode>(2);
		xor_switch_gate_node->set_output(option1_node, 1);

		auto option2_node = std::make_shared<EndNode>(3);
		xor_switch_gate_node->set_output(option2_node, 2);

		auto lookup_func = [](const time::time_t &time,
		                      const std::shared_ptr<GameEntity> & /* entity */,
		                      const std::shared_ptr<gamestate::GameState> & /* state */,
		                      const std::shared_ptr<nyan::Object> & /* api_object */) {
			if (time == time::TIME_ZERO) {
				return 1;
			}
			if (time == time::TIME_MAX) {
				return 2;
			}

			return 0;
		};
		// we don't pass a nyan::Object as it's not used by the switch function
		xor_switch_gate_node->set_switch_func({nullptr, lookup_func});

		// Check the switch function
		// we don't pass GameEntity or GameState as it's not used by the switch functions
		auto switch_condition = xor_switch_gate_node->get_switch_func();
		auto switch_obj = switch_condition.api_object;
		auto switch_func = switch_condition.function;
		TESTEQUALS(switch_func(time::TIME_ZERO, nullptr, nullptr, switch_obj), 1);
		TESTEQUALS(switch_func(time::TIME_MAX, nullptr, nullptr, switch_obj), 2);
		TESTEQUALS(switch_func(time::TIME_MIN, nullptr, nullptr, switch_obj), 0);

		auto lookup_dict = xor_switch_gate_node->get_lookup_dict();

		// Check the lookup dict
		TESTEQUALS(lookup_dict.size(), 2);

		TESTEQUALS(lookup_dict.at(1), option1_node);
		TESTEQUALS(lookup_dict.at(2), option2_node);

		// Check if next nodes return correctly
		TESTEQUALS(xor_switch_gate_node->next(1), default_node);
		TESTEQUALS(xor_switch_gate_node->next(2), option1_node);
		TESTEQUALS(xor_switch_gate_node->next(3), option2_node);

		// Check that the node throws errors for invalid output IDs
		TESTTHROWS(xor_switch_gate_node->next(999));
	}

	// Xor event gate node type
	{
		auto xor_event_gate_node = std::make_shared<XorEventGate>(0);

		// Check basic properties inherited from Node interface
		TESTEQUALS(xor_event_gate_node->get_id(), 0);
		// TESTEQUALS(xor_event_gate_node->get_type(), node_t::XOR_EVENT_GATE);
		TESTEQUALS(xor_event_gate_node->get_label(), "ExclusiveEventGateway");
		TESTEQUALS(xor_event_gate_node->str(), "ExclusiveEventGateway (id=0)");

		auto option1_node = std::make_shared<EndNode>(1);
		xor_event_gate_node->add_output(option1_node, [](const time::time_t & /* time */, const std::shared_ptr<GameEntity> & /* entity */, const std::shared_ptr<event::EventLoop> & /* loop */, const std::shared_ptr<GameState> & /* state */, size_t /* next_id */) {
			return nullptr;
		});

		auto option2_node = std::make_shared<EndNode>(2);
		xor_event_gate_node->add_output(option2_node, [](const time::time_t & /* time */, const std::shared_ptr<GameEntity> & /* entity */, const std::shared_ptr<event::EventLoop> & /* loop */, const std::shared_ptr<GameState> & /* state */, size_t /* next_id */) {
			return nullptr;
		});

		auto primers = xor_event_gate_node->get_primers();

		// Check the primers
		TESTEQUALS(primers.size(), 2);

		// Check if next nodes return correctly
		TESTEQUALS(xor_event_gate_node->next(1), option1_node);
		TESTEQUALS(xor_event_gate_node->next(2), option2_node);

		// Check that the node throws errors for invalid output IDs
		TESTTHROWS(xor_event_gate_node->next(999));
	}
}

} // namespace openage::gamestate::activity::tests
