// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include <cstddef>
#include <functional>
#include <memory>
#include <string>

#include "event/event_loop.h"
#include "event/evententity.h"
#include "event/eventhandler.h"
#include "event/state.h"

#include "error/error.h"
#include "log/log.h"
#include "log/message.h"

#include "gamestate/activity/end_node.h"
#include "gamestate/activity/event_node.h"
#include "gamestate/activity/node.h"
#include "gamestate/activity/start_node.h"
#include "gamestate/activity/task_node.h"
#include "gamestate/activity/types.h"
#include "gamestate/activity/xor_node.h"
#include "time/time.h"


namespace openage::gamestate::tests {


/**
 * Main control flow that navigates through the activity node graph.
 *
 * @param current_node Node where the control flow starts from.
 * @return Node where the control flow should continue.
 */
const std::shared_ptr<activity::Node> activity_flow(const std::shared_ptr<activity::Node> &current_node);


/**
 * Notified by event loop when the activity flow starts/resumes.
 *
 * It keeps track of the current node in the activity flow graph.
 */
class TestActivityManager : public event::EventEntity {
public:
	TestActivityManager(const std::shared_ptr<event::EventLoop> &loop,
	                    const std::shared_ptr<activity::Node> &start_node) :
		EventEntity{loop},
		current_node{start_node} {
	}

	size_t id() const override {
		return 0;
	}

	std::string idstr() const override {
		return "TestActivityManager";
	}

	void run() {
		if (not current_node) {
			throw Error{ERR << "No current node given"};
		}
		this->current_node = activity_flow(this->current_node);
	}

	std::shared_ptr<activity::Node> current_node;
};

/**
 * Event state. Unused but required by the event system.
 */
class TestActivityState : public event::State {
public:
	using event::State::State;
};


/**
 * Receives the event when the activity flow and notifies the activity manager
 * to continue the flow.
 */
class TestActivityHandler : public event::OnceEventHandler {
public:
	using event::OnceEventHandler::OnceEventHandler;

	void setup_event(const std::shared_ptr<event::Event> & /* event */,
	                 const std::shared_ptr<event::State> & /* state */) override {
		return;
	}

	void invoke(event::EventLoop & /* loop */,
	            const std::shared_ptr<event::EventEntity> &target,
	            const std::shared_ptr<event::State> & /* state */,
	            const time::time_t & /* time */,
	            const param_map & /* params */) override {
		auto mgr_target = std::dynamic_pointer_cast<TestActivityManager>(target);
		mgr_target->run();
	}

	time::time_t predict_invoke_time(const std::shared_ptr<event::EventEntity> & /* target */,
	                                 const std::shared_ptr<event::State> & /* state */,
	                                 const time::time_t &at) override {
		return at;
	}
};


const std::shared_ptr<activity::Node> activity_flow(const std::shared_ptr<activity::Node> &current_node) {
	auto current = current_node;

	if (current->get_type() == activity::node_t::XOR_EVENT_GATE) {
		auto node = std::static_pointer_cast<activity::XorEventGate>(current);
		auto event_next = node->get_next_func();
		auto next_id = event_next(0, nullptr, nullptr, nullptr);
		current = node->next(next_id);
	}

	while (current->get_type() != activity::node_t::END) {
		log::log(INFO << "Visiting node: " << current->str());

		switch (current->get_type()) {
		case activity::node_t::START: {
			auto node = std::static_pointer_cast<activity::StartNode>(current);
			auto next_id = node->get_next();
			current = node->next(next_id);
		} break;
		case activity::node_t::END: {
			// TODO
			return current;
		} break;
		case activity::node_t::TASK_CUSTOM: {
			auto node = std::static_pointer_cast<activity::TaskCustom>(current);
			auto task = node->get_task_func();
			task(0, nullptr);
			auto next_id = node->get_next();
			current = node->next(next_id);
		} break;
		case activity::node_t::XOR_GATE: {
			auto node = std::static_pointer_cast<activity::XorGate>(current);
			auto condition = node->get_condition_func();
			auto next_id = condition(0, nullptr);
			current = node->next(next_id);
		} break;
		case activity::node_t::XOR_EVENT_GATE: {
			auto node = std::static_pointer_cast<activity::XorEventGate>(current);
			auto event_primer = node->get_primer_func();
			event_primer(0, nullptr, nullptr, nullptr);

			// wait for event
			return current;
		} break;
		default:
			throw Error{ERR << "Unhandled node type for node " << current->str()};
		}

		log::log(INFO << "Next node: " << current->str());
	}

	log::log(INFO << "Reached end note: " << current->str());

	return current;
}


/**
 * Main test. Sets up the activity graph.
 *
 *
 * Graph:
 * Start -> Task 1 -> XOR -> Event -> Task 2 -> End
 *            ^--------|
 */
void activity_demo() {
	// create an event loop for events in the graph
	auto loop = std::make_shared<event::EventLoop>();
	auto handler = std::make_shared<TestActivityHandler>("test.activity");
	loop->add_event_handler(handler);
	auto state = std::make_shared<TestActivityState>(loop);

	// create the nodes in the graph
	// connections are created further below
	auto start = std::make_shared<activity::StartNode>(0);
	auto task1 = std::make_shared<activity::TaskCustom>(1);
	auto xor_node = std::make_shared<activity::XorGate>(2);
	auto event_node = std::make_shared<activity::XorEventGate>(3);
	auto task2 = std::make_shared<activity::TaskCustom>(4);
	auto end = std::make_shared<activity::EndNode>(5);

	// create an activity manager that controls the flow
	auto mgr = std::make_shared<TestActivityManager>(loop, start);

	// connect the nodes

	// start node
	start->add_output(task1);

	// task 1
	task1->add_output(xor_node);
	task1->set_task_func([](const time::time_t & /* time */,
	                        const std::shared_ptr<gamestate::GameEntity> & /* entity */) {
		log::log(INFO << "Running task 1");
	});

	// Conditional branch
	size_t counter = 0;
	xor_node->add_output(task1);
	xor_node->add_output(event_node);
	xor_node->set_condition_func([&](const time::time_t & /* time */,
	                                 const std::shared_ptr<gamestate::GameEntity> & /* entity */) {
		log::log(INFO << "Checking condition (counter < 4): counter=" << counter);
		if (counter < 4) {
			log::log(INFO << "Selecting path 1 (back to task node " << task1->get_id() << ")");

			counter++;
			return task1->get_id();
		}

		log::log(INFO << "Selecting path 2 (to event node " << event_node->get_id() << ")");

		return event_node->get_id();
	});

	// event node
	event_node->add_output(task2);
	event_node->set_primer_func([&](const time::time_t & /* time */,
	                                const std::shared_ptr<gamestate::GameEntity> & /* entity */,
	                                const std::shared_ptr<event::EventLoop> & /* loop */,
	                                const std::shared_ptr<gamestate::GameState> & /* state */) {
		log::log(INFO << "Setting up event");
		auto ev = loop->create_event("test.activity",
		                             mgr,
		                             state,
		                             0);
		return activity::event_store_t{ev};
	});
	event_node->set_next_func([&task2](const time::time_t & /* time */,
	                                   const std::shared_ptr<gamestate::GameEntity> & /* entity */,
	                                   const std::shared_ptr<event::EventLoop> & /* loop */,
	                                   const std::shared_ptr<gamestate::GameState> & /* state */) {
		log::log(INFO << "Selecting next node (task node " << task2->get_id() << ")");
		return task2->get_id();
	});

	// task 2
	task2->add_output(end);
	task2->set_task_func([](const time::time_t & /* time */,
	                        const std::shared_ptr<gamestate::GameEntity> & /* entity */) {
		log::log(INFO << "Running task 2");
	});

	// run the manager to start from the start node
	// this will run until the event node is reached
	mgr->run();

	// process events
	// this will call back the activity manager to continue the flow
	loop->reach_time(0, state);
}

} // namespace openage::gamestate::tests
