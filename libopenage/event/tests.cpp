// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#include <cstring> // for strcmp
#include <iostream>
#include <utility>

#include "../testing/testing.h"
#include "../log/log.h"

#include "loop.h"
#include "event.h"
#include "eventtarget.h"
#include "state.h"


namespace openage::event::tests {

// We have to create a temporary State due to the magic of C++
class TestState : public State {
public:
	class TestObject : public EventTarget {
		const int _id;
	public:
		TestObject(const std::shared_ptr<Loop> &loop, int id)
			:
			EventTarget(loop),
			_id{id},
			number(0) {}

		void set_number(int number, const curve::time_t &time) {
			this->number = number;
			this->changes(time + curve::time_t::from_double(1));
		}

		[[nodiscard]] size_t id() const override {
			return _id;
		}

		void test_trigger(const curve::time_t &time) {
			this->trigger(time);
		}

		int number;
	};

	explicit TestState(const std::shared_ptr<Loop> &loop)
		:
		State(loop),
		objectA(std::make_shared<TestObject>(loop, 0)) ,
		objectB(std::make_shared<TestObject>(loop, 1)) {}

	std::shared_ptr<TestObject> objectA;
	std::shared_ptr<TestObject> objectB;

	struct traceelement {
		traceelement(std::string event, curve::time_t time)
			:
			time{std::move(time)},
			name{std::move(event)} {}

		curve::time_t time;
		std::string name;
	};

	std::list<traceelement> trace;

	void log_dbg() {
		log::log(DBG << "Trace: ");
		for (const auto &e : this->trace) {
			log::log(DBG << "T: " << e.time << ": " << e.name);
		}
	}
};


class TestEventClass : public EventClass {
	int idx;
public:
	TestEventClass(const std::string &name, int idx)
		:
		EventClass(name, EventClass::trigger_type::DEPENDENCY),
		idx{idx} {}

	void setup_event(const std::shared_ptr<Event> &event,
	                 const std::shared_ptr<State> &gstate) override {

		auto state = std::dynamic_pointer_cast<TestState>(gstate);

		switch(this->idx) {
		case 0:
			// let the modification of objectA depend on objectB
			event->depend_on(state->objectB);
			break;
		case 1:
			// let the modification of objectB depend on objectA
			event->depend_on(state->objectA);
			break;
		}
	}

	void invoke(Loop &/*loop*/,
	            const std::shared_ptr<EventTarget> &target,
	            const std::shared_ptr<State> &gstate,
	            const curve::time_t &time,
	            const EventClass::param_map &/*param*/) override {

		auto state = std::dynamic_pointer_cast<TestState>(gstate);

		switch (this->idx) {
		case 0: {
			auto t = std::dynamic_pointer_cast<TestState::TestObject>(target);
			state->objectA->set_number(t->number + 1, time);
			log::log(DBG << "I am Event A. Setting number to " << state->objectA->number);
			state->trace.emplace_back("A", time);
		} break;

		case 1: {
			auto t = std::dynamic_pointer_cast<TestState::TestObject>(target);
			state->objectB->set_number(t->number + 1, time);
			log::log(DBG << "I am Event B. Setting number to " << state->objectB->number);
			state->trace.emplace_back("B", time);
		} break;
		}
	}

	curve::time_t predict_invoke_time(const std::shared_ptr<EventTarget> &/*target*/,
	                                  const std::shared_ptr<State> &/*state*/,
	                                  const curve::time_t &at) override {
		return at + curve::time_t::from_double(2);
	}
};


class TestEventClassTwo : public EventClass {
public:
	explicit TestEventClassTwo(const std::string &name)
		:
		EventClass(name, EventClass::trigger_type::DEPENDENCY) {}

	void setup_event(const std::shared_ptr<Event> &target,
	                 const std::shared_ptr<State> &gstate) override {

		auto state = std::dynamic_pointer_cast<TestState>(gstate);
		target->depend_on(state->objectA);
	}

	void invoke(Loop &/*loop*/,
	            const std::shared_ptr<EventTarget> &gtarget,
	            const std::shared_ptr<State> &gstate,
	            const curve::time_t &time,
	            const EventClass::param_map &/*param*/) override {

		auto state = std::dynamic_pointer_cast<TestState>(gstate);
		auto target = std::dynamic_pointer_cast<TestState::TestObject>(gtarget);
		state->objectB->set_number(target->number + 1, time);

		log::log(DBG << "I am EventClassTwo. Setting B.number to " << state->objectB->number);
		state->trace.emplace_back("B", time);
	}

	curve::time_t predict_invoke_time(const std::shared_ptr<EventTarget> &/*target*/,
	                                  const std::shared_ptr<State> &/*state*/,
	                                  const curve::time_t &at) override {
		// TODO recalculate a hit time
		return at + curve::time_t::from_double(1);
	}
};


class EventTypeTestClass : public EventClass {
public:
	EventTypeTestClass(const std::string &name, EventClass::trigger_type type)
		:
		EventClass(name, type) {}

	void setup_event(const std::shared_ptr<Event> &event,
	                 const std::shared_ptr<State> &gstate) override {

		log::log(DBG << "EventTypeTestClass-" << this->id() << " setting up new event");

		// let all events depend on objectA
		auto state = std::dynamic_pointer_cast<TestState>(gstate);
		event->depend_on(state->objectA);
	}

	void invoke(Loop &/*loop*/,
	            const std::shared_ptr<EventTarget> &target,
	            const std::shared_ptr<State> &gstate,
	            const curve::time_t &time,
	            const EventClass::param_map &/*param*/) override {

		auto state = std::dynamic_pointer_cast<TestState>(gstate);

		auto t = std::dynamic_pointer_cast<TestState::TestObject>(target);
		log::log(DBG << "EventTypeTestClass-" << this->id() << " got called on " << t->id()
		         << " with number " << t->number << " at t=" << time);

		state->trace.emplace_back(this->id(), time);
	}

	curve::time_t predict_invoke_time(const std::shared_ptr<EventTarget> &/*target*/,
	                                  const std::shared_ptr<State> &/*state*/,
	                                  const curve::time_t &at) override {
		switch(this->type) {
		case EventClass::trigger_type::DEPENDENCY:
			// Execute 1 after the change (usually it is neccessary to recalculate a collision
			return at + curve::time_t::from_double(1);

		case EventClass::trigger_type::DEPENDENCY_IMMEDIATELY:
			TESTFAILMSG("DEPENDENCY_IMMEDIATELY does not recalculate time!");
			return 0;

		case EventClass::trigger_type::TRIGGER:
			TESTFAILMSG("TRIGGER does not recalculate time!");
			return 0;

		case EventClass::trigger_type::REPEAT:
			// This will force the execution every 5ms
			return at + curve::time_t::from_double(5);

		case EventClass::trigger_type::ONCE:
			return 10;  // even if data changed it will happen at the given time!
		}
		return at;
	}
};


void eventtrigger() {
	log::log(DBG << "------------- [ Starting Test: Basic Ping Pong ] ------------");

	// test destruction
	std::weak_ptr<TestState> destruction_test_state;

	{
		// Test with one event class
		auto loop = std::make_shared<Loop>();

		loop->add_event_class(std::make_shared<TestEventClass>("test_on_A", 0));
		loop->add_event_class(std::make_shared<TestEventClass>("test_on_B", 1));

		auto state = std::make_shared<TestState>(loop);
		auto gstate = std::static_pointer_cast<State>(state);

		destruction_test_state = state;

		// One must not start the game at 0 - this leads to randomness in execution
		loop->create_event("test_on_B", state->objectB, gstate, 1);
		loop->create_event("test_on_A", state->objectA, gstate, 1);

		// It is expected, that A and B hand over the "changed" events between each other
		state->objectA->set_number(0, 0);

		// run 10 iterations, for times 2, 4, ... 20
		for (int i = 0; i < 10; ++i) {
			loop->reach_time((i + 1) * 2, gstate);
		}

		state->log_dbg();

		if (state->trace.size() < 6) {
			TESTFAILMSG("not enough items collected");
		}

		int i = 0;

		curve::time_t last_time = 0;
		for (const auto &e : state->trace) {

			if (last_time > e.time) {
				TESTFAILMSG("You broke the time continuum: one shall not execute randomly!");
			}

			last_time = e.time;
			switch(i) {
			case 0: TESTEQUALS(e.name, "B"); TESTEQUALS(e.time, 3); break;
			case 1: TESTEQUALS(e.name, "A"); TESTEQUALS(e.time, 6); break;
			case 2: TESTEQUALS(e.name, "B"); TESTEQUALS(e.time, 9); break;
			case 3: TESTEQUALS(e.name, "A"); TESTEQUALS(e.time, 12); break;
			case 4: TESTEQUALS(e.name, "B"); TESTEQUALS(e.time, 15); break;
			case 5: TESTEQUALS(e.name, "A"); TESTEQUALS(e.time, 18); break;
			default: TESTFAILMSG("Too many elements in stack trace"); break;
			}

			i += 1;
		}
	}

	if (!destruction_test_state.expired()) {
		TESTFAILMSG("Test Failed because TestState was not automatically destructed");
	}

	log::log(DBG << "------------- [ Starting Test: Two Event Ping Pong ] ------------");
	{
		// Test with two event classes to check interplay
		auto loop = std::make_shared<Loop>();

		loop->add_event_class(std::make_shared<TestEventClass>("test_on_A", 0));
		loop->add_event_class(std::make_shared<TestEventClassTwo>("test_on_B"));

		auto state = std::make_shared<TestState>(loop);
		auto gstate = std::static_pointer_cast<State>(state);

		// One must not start the game at 0 - this leads to randomness in execution
		loop->create_event("test_on_B", state->objectB, gstate, 1);
		loop->create_event("test_on_A", state->objectA, gstate, 1);

		// It is expected, that A and B hand over the "changed" events between each other
		state->objectA->set_number(0, 1);

		// run 10 iterations, for times 2, 4, ... 20
		for (int i = 0; i < 10; ++i) {
			loop->reach_time((i + 1) * 2, gstate);
		}

		state->log_dbg();

		if (state->trace.size() < 7) {
			TESTFAILMSG("not enough items collected");
		}

		int i = 0;
		curve::time_t last_time = 0;

		for (const auto &e : state->trace) {
			if (last_time > e.time) {
				TESTFAILMSG("You broke the time continuum: one shall not execute randomly!");
			}

			last_time = e.time;
			switch(i) {
			case 0: TESTEQUALS(e.name, "B"); TESTEQUALS(e.time, 3); break;
			case 1: TESTEQUALS(e.name, "A"); TESTEQUALS(e.time, 6); break;
			case 2: TESTEQUALS(e.name, "B"); TESTEQUALS(e.time, 8); break;
			case 3: TESTEQUALS(e.name, "A"); TESTEQUALS(e.time, 11); break;
			case 4: TESTEQUALS(e.name, "B"); TESTEQUALS(e.time, 13); break;
			case 5: TESTEQUALS(e.name, "A"); TESTEQUALS(e.time, 16); break;
			case 6: TESTEQUALS(e.name, "B"); TESTEQUALS(e.time, 18); break;
			default: TESTFAILMSG("Too many elements in stack trace"); break;
			}

			i += 1;
		}
	}

	log::log(DBG << "------------- [ Starting Test: Complex Event Types ] ------------");
	// Now set up a more complex test to test the different event types
	{
		auto loop = std::make_shared<Loop>();

		loop->add_event_class(std::make_shared<EventTypeTestClass>(
			                         "object_modify",
			                         EventClass::trigger_type::DEPENDENCY));
		loop->add_event_class(std::make_shared<EventTypeTestClass>(
			                         "object_modify_immediately",
			                         EventClass::trigger_type::DEPENDENCY_IMMEDIATELY));
		loop->add_event_class(std::make_shared<EventTypeTestClass>(
			                         "object_trigger",
			                         EventClass::trigger_type::TRIGGER));
		loop->add_event_class(std::make_shared<EventTypeTestClass>(
			                         "repeat_exec",
			                         EventClass::trigger_type::REPEAT));
		loop->add_event_class(std::make_shared<EventTypeTestClass>(
			                         "once",
			                         EventClass::trigger_type::ONCE));

		auto state = std::make_shared<TestState>(loop);
		auto gstate = std::static_pointer_cast<State>(state);

		// One must not start the game at 0 - this leads to randomness in execution

		// -------------------------------------
		// Add some events to be invoked:

		// execs whenever objectA is changed + 1
		loop->create_event("object_modify", state->objectA, gstate, 4);

		// execs whenever objectA is changed + 1,
		// does not use a newly predicted time as object_modify_immediately
		loop->create_event("object_modify_immediately", state->objectA, gstate, 1);

		// execs at t=x when test_trigger(x) is invoked on objectA
		loop->create_event("object_trigger", state->objectA, gstate, 1);

		// execs periodically at t=n*5
		loop->create_event("repeat_exec", state->objectA, gstate, 0);

		// executes just once at t=10
		loop->create_event("once", state->objectA, gstate, 10);

		log::log(DBG << "##### SETUP DONE ");

		// Without anything happening and until time 0, nothing will happen
		{
			// Expected: object_modify_immediately(t=1) [ we changed data at t=1 ]
			loop->reach_time(0, gstate);
			TESTEQUALS(state->trace.empty(), true);
		}

		state->objectA->set_number(0, 1);
		{
			// Evaluate the state until t=2
			// Expected: object_modify_immediately(t=(1+2)),
			// because we set the number at t=1,
			// which then creates a invoke time of t+1 == 2
			loop->reach_time(2, gstate);
			TESTEQUALS(state->trace.size(), 1);
			TESTEQUALS(state->trace.front().name, "object_modify_immediately");
			TESTEQUALS(state->trace.front().time, 2);
			state->trace.clear();
		}

		{
			// executing to t=2 again should yield nothing,
			// we reached t=2 in the step before already
			loop->reach_time(2, gstate);
			TESTEQUALS(state->trace.empty(), true);
		}

		log::log(DBG << "##### INIT DONE ");
		log::log(DBG << "Triggering Keyframe at 1");

		state->objectA->test_trigger(1);
		{
			// Expected: object_trigger(1)
			loop->reach_time(2, gstate);
			state->log_dbg();
			TESTEQUALS(state->trace.size(), 1);
			if (state->trace.front().name != "object_trigger")
				TESTFAILMSG("Unexpected Event: " << state->trace.front().name
				            << ", expected object_trigger");

			TESTEQUALS(state->trace.front().time, 1);
			state->trace.clear();
		}
		{
			// Expected: object_modify(1+1+1=3) and repeat_exec(5)
			loop->reach_time(5, gstate);

			TESTEQUALS(state->trace.size(), 2);
			auto it = state->trace.begin();
			if (it->name != "object_modify")
				TESTFAILMSG("Unexpected Event: " << it->name << " expected object_modify");
			TESTEQUALS(it->time, 3);
			it++;
			if (it->name != "repeat_exec")
				TESTFAILMSG("Unexpected Event: " << it->name << " expected repeat_exec");
			TESTEQUALS(it->time, 5);
			state->trace.clear();
		}
		{
			// Expected: repeat_exec(10), once(10)
			loop->reach_time(11, gstate);
			TESTEQUALS(state->trace.size(), 2);
			auto it = state->trace.begin();
			if (it->name != "repeat_exec")
				TESTFAILMSG("Unexpected Event: " << it->name << " expected repeat_exec");
			TESTEQUALS(it->time, 10);
			it++;
			if (it->name != "once")
				TESTFAILMSG("Unexpected Event: " << it->name << " expected once");
			TESTEQUALS(it->time, 10);
			state->trace.clear();
		}

		log::log(DBG << "changing the value at t=12");
		state->objectA->set_number(1, 12);
		{
			// Expected: object_modify_immediately(12+1=13), object_modify(12+1+1=14)
			// object_modify_immediately is executed at the time of change (12+1)
			// whereas object_modify is executed with a newly predicted time (that is +1)
			loop->reach_time(15, gstate);
			TESTEQUALS(state->trace.size(), 3);
			auto it = state->trace.begin();
			if (it->name != "object_modify_immediately")
				TESTFAILMSG("Unexpected Event: " << it->name << " expected object_modify_immediately");
			TESTEQUALS(it->time, 13);
			it++;
			if (it->name != "object_modify")
				TESTFAILMSG("Unexpected Event: " << it->name << " expected object_modify");
			TESTEQUALS(it->time, 14);
			it++;
			if (it->name != "repeat_exec")
				TESTFAILMSG("Unexpected Event: " << it->name << " expected repeat_exec");
			TESTEQUALS(it->time, 15);
			state->trace.clear();
		}
	}

	log::log(DBG << "------------- [ Starting Test: Event parameter Mapping ] ------------");
	{
		class EventParameterMapTestClass : public EventClass {
		public:
			EventParameterMapTestClass()
				:
				EventClass("EventParameterMap", EventClass::trigger_type::ONCE) {}

			void setup_event(const std::shared_ptr<Event> &/*target*/,
			                 const std::shared_ptr<State> &/*state*/) override {}

			void invoke(Loop &/*loop*/,
			            const std::shared_ptr<EventTarget> &/*target*/,
			            const std::shared_ptr<State> &/*state*/,
			            const curve::time_t &/*time*/,
			            const EventClass::param_map &param) override {

				log::log(DBG << "Testing unknown parameter");
				TESTEQUALS(param.contains("tomato"), false);
				TESTEQUALS(param.check_type<int>("tomato"), false);
				TESTEQUALS(param.get<int>("tomato", 1), 1);
				TESTEQUALS(param.get<int>("tomato"), 0);
				TESTEQUALS(param.get<std::string>("tomato", "test"), "test");
				TESTEQUALS(param.get<std::string>("tomato"), "");

				log::log(DBG << "Testing Integer parameter");
				TESTEQUALS(param.contains("testInt"), true);
				TESTEQUALS(param.check_type<int>("testInt"), true);
				TESTEQUALS(param.get<int>("testInt"), 1);
				// FIXME: This should hurt you!!!
				TESTEQUALS(param.get<std::string>("testInt", "int"), "int");

				log::log(DBG << "Testing char* parameter");
				TESTEQUALS(param.contains("testString"), true);
				TESTEQUALS(param.check_type<const char*>("testString"), true);
				TESTEQUALS(strcmp(param.get<const char*>("testString"), "string"), 0);

				log::log(DBG << "Testing std::string parameter");
				TESTEQUALS(param.contains("testStdString"), true);
				TESTEQUALS(param.check_type<std::string>("testStdString"), true);
				TESTEQUALS(param.get<std::string>("testStdString"), "stdstring");
			}

			curve::time_t predict_invoke_time(const std::shared_ptr<EventTarget> &/*target*/,
			                                  const std::shared_ptr<State> &/*state*/,
			                                  const curve::time_t &at) override {
				return at;
			}
		};

		using namespace std::literals;

		auto loop = std::make_shared<Loop>();
		loop->add_event_class(std::make_shared<EventParameterMapTestClass>());
		auto state = std::make_shared<TestState>(loop);
		auto gstate = std::dynamic_pointer_cast<State>(state);

		loop->create_event("EventParameterMap", state->objectA, gstate, 1, {
				{"testInt", 1},
				{"testStdString", "stdstring"s},
				{"testString", "string"}
			}
		);
		loop->reach_time(10, gstate);
	}
}

} // namespace openage::event::tests
