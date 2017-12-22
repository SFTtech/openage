// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "../events/events.h"
#include "../events/eventtarget.h"
#include "../events/eventclass.h"
#include "../events/eventmanager.h"


#include "../../testing/testing.h"
#include "../../log/log.h"

#include <iostream>

#include <string.h> // for strcmp

namespace openage {

// We have to create a temporary State due to the magic of C++
class State {
public:
	class TestObject : public curve::EventTarget {
		const int _id;
	public:
		TestObject(curve::EventManager *manager, int id) : EventTarget(manager), _id{id}, number(0) {}
		void set_number(int number, const curve::curve_time_t &time) {
			this->number = number;
			this->on_change(time + 1);
		}
		int number;

		size_t id() const override { return _id; }
		void trigger_keyframe(const curve::curve_time_t &time) { this->on_pass_keyframe(time); }
	};

	State(curve::EventManager *manager) :
		objectA(std::make_shared<TestObject>(manager, 0)) ,
		objectB(std::make_shared<TestObject>(manager, 1))
        {}

	std::shared_ptr<TestObject> objectA;
	std::shared_ptr<TestObject> objectB;

	struct traceelement {
		traceelement(const std::string &event, curve::curve_time_t time) :
			time{time}, name{event} {}
		curve::curve_time_t time;
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

namespace curve {
namespace tests {

class TestEventClass : public EventClass {
	int idx;
public:
	TestEventClass(const std::string &name, int idx) : EventClass(name, EventClass::Type::ON_CHANGE), idx{idx} {}

	void setup(const std::shared_ptr<Event> &target,
	           const std::shared_ptr<State> &state) override {
		switch(idx) {
		case 0:
			add_dependency(target, state->objectB);
			break;
		case 1:
			add_dependency(target, state->objectA);
			break;
		}
	}

	void call(EventManager *,
	          const std::shared_ptr<EventTarget> &target,
	          const std::shared_ptr<State> &state,
	          const curve_time_t &time,
              const EventClass::param_map &/*param*/) override {
		(void)target;
		switch (idx) {
		case 0: {
			auto t = std::dynamic_pointer_cast<State::TestObject>(target);
			state->objectA->set_number(t->number + 1, time);
			log::log(DBG << "I am Event A. Setting number to " << state->objectA->number);
			state->trace.emplace_back("A", time);
		} break;
		case 1: {
			auto t = std::dynamic_pointer_cast<State::TestObject>(target);
			state->objectB->set_number(t->number + 1, time);
			log::log(DBG << "I am Event B. Setting number to " << state->objectB->number);
			state->trace.emplace_back("B", time);
		} break;
		}
	}

	curve_time_t recalculate_time(const std::shared_ptr<EventTarget> &target,
	                              const std::shared_ptr<State> &state,
	                              const curve::curve_time_t &at) override {
		// TODO recalculate a hit time
		(void)target;
		(void)state;
		return at + 2;
	}
};

class TestEventClassTwo : public EventClass {
public:
	TestEventClassTwo(const std::string &name) : EventClass(name, EventClass::Type::ON_CHANGE) {}

	void setup(const std::shared_ptr<Event> &target, const std::shared_ptr<State> &state) override {
		add_dependency(target, state->objectA);
	}

	void call(EventManager *,
			  const std::shared_ptr<EventTarget> &target,
			  const std::shared_ptr<State> &state,
			  const curve_time_t &time,
              const EventClass::param_map &/*param*/) override {
		auto t = std::dynamic_pointer_cast<State::TestObject>(target);
		state->objectB->set_number(t->number + 1, time);
		log::log(DBG << "I am EventClassTwo. Setting B.number to " << state->objectB->number);
		state->trace.emplace_back("B", time);
	}

	curve_time_t recalculate_time(const std::shared_ptr<EventTarget> &target,
								  const std::shared_ptr<State> &state,
								  const curve::curve_time_t &at) override {
		// TODO recalculate a hit time
		(void)target;
		(void)state;
		return at + 1;
	}
};

class EventTypeTestClass : public EventClass {
public:
	EventTypeTestClass(const std::string &name, EventClass::Type type) : EventClass(name, type) {}

	void setup(const std::shared_ptr<Event> &target,
	           const std::shared_ptr<State> &state) override {
		add_dependency(target, state->objectA);
	}

	void call(EventManager *,
	          const std::shared_ptr<EventTarget> &target,
	          const std::shared_ptr<State> &state,
	          const curve_time_t  &time,
              const EventClass::param_map &/*param*/) override {
		switch(this->type) {
		case EventClass::Type::ON_CHANGE:
		case EventClass::Type::ON_CHANGE_IMMEDIATELY:
		case EventClass::Type::ON_KEYFRAME:
		case EventClass::Type::ON_EXECUTE:
		case EventClass::Type::ONCE:
			break;
		}
		auto t = std::dynamic_pointer_cast<State::TestObject>(target);
		log::log(DBG << this->id() << " got called on " << t->id()
		         << " with number " << t->number << " at " << time);
		state->trace.emplace_back(this->id(), time);
		(void)time;
		(void)state;
	}

	curve_time_t recalculate_time(const std::shared_ptr<EventTarget> &target,
	                              const std::shared_ptr<State> &state,
	                              const curve::curve_time_t &at) override {
		(void)target;
		(void)state;
		switch(this->type) {
		case EventClass::Type::ON_CHANGE:
			return at+1; // Execute 1 after the change (usually it is neccessary to recalculate a colission
		case EventClass::Type::ON_CHANGE_IMMEDIATELY:
			TESTFAILMSG("ON_CHANGE_IMMEDIATELY does not recalculate time!");
			return 0; // This is ignored anyway
		case EventClass::Type::ON_KEYFRAME:
			TESTFAILMSG("ON_KEYFRAME does not recalculate time!");
			return 0; // This is ignored anyway
		case EventClass::Type::ON_EXECUTE:
			return at + 5; // This will force the execution every 5ms
		case EventClass::Type::ONCE:
			return 10; // even if data changed it will happen at the given time!
		}
		return at;
	}
};

void events2() {
	log::log(DBG << "------------- [ Starting Test: Basic Ping Pong ] ------------");
	// test destruction
	std::weak_ptr<State> destruction_test_state;
	{
		// Test with one event class
		EventManager manager;

		manager.add_class(std::make_shared<TestEventClass>("testOnA", 0));
		manager.add_class(std::make_shared<TestEventClass>("testOnB", 1));

		auto state = std::make_shared<State>(&manager);
		destruction_test_state = state;
		// One must not start the game at 0 - this leads to randomness in execution
		manager.on("testOnB", state->objectB, state, 1);
		manager.on("testOnA", state->objectA, state, 1);

		// It is expected, that A and B hand over the "changed" events between each other
		state->objectA->set_number(0, 0);
		for (int i = 1; i < 10; ++i) {
			manager.execute_until(i * 2, state);
		}

		state->log_dbg();

		int i = 0;
		curve_time_t last_time = 0;
		for (auto it = state->trace.begin(); it != state->trace.end(); ++it, ++i) {
			const auto &e = *it;
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
			default: TESTFAILMSG("Too many elements in stack trace"); break;
			}
		}
	}
	if (!destruction_test_state.expired()) {
		TESTFAILMSG("Test Failed because State was not automatically destructed");
	}

	log::log(DBG << "------------- [ Starting Test: Two Event Ping Pong ] ------------");
	{
		// Test with two event classes to check interplay
		// Test with one event class
		EventManager manager;

		manager.add_class(std::make_shared<TestEventClass>("testOnA", 0));
		manager.add_class(std::make_shared<TestEventClassTwo>("testOnB"));

		auto state = std::make_shared<State>(&manager);
		// One must not start the game at 0 - this leads to randomness in execution
		manager.on("testOnB", state->objectB, state, 1);
		manager.on("testOnA", state->objectA, state, 1);

		// It is expected, that A and B hand over the "changed" events between each other
		state->objectA->set_number(0, 1);
		for (int i = 1; i < 10; ++i) {
			manager.execute_until(i * 2, state);
		}

		state->log_dbg();

		int i = 0;
		curve_time_t last_time = 0;
		for (auto it = state->trace.begin(); it != state->trace.end(); ++it, ++i) {
			const auto &e = *it;
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
			default: TESTFAILMSG("Too many elements in stack trace"); break;
			}
		}
	}

	log::log(DBG << "------------- [ Starting Test: Complex Event Types ] ------------");
	// Now set up a more complex test to test the different event types
	{
		EventManager manager;
		manager.add_class(std::make_shared<EventTypeTestClass>(
			                  "onChange",
			                  EventClass::Type::ON_CHANGE));
		manager.add_class(std::make_shared<EventTypeTestClass>(
			                  "onChangeImm",
			                  EventClass::Type::ON_CHANGE_IMMEDIATELY));
		manager.add_class(std::make_shared<EventTypeTestClass>(
			                  "onKeyframe",
			                  EventClass::Type::ON_KEYFRAME));
		manager.add_class(std::make_shared<EventTypeTestClass>(
			                  "onExecute",
			                  EventClass::Type::ON_EXECUTE));
		manager.add_class(std::make_shared<EventTypeTestClass>(
			                  "once",
			                  EventClass::Type::ONCE));

		auto state = std::make_shared<State>(&manager);

		// One must not start the game at 0 - this leads to randomness in execution
		manager.on("onChange", state->objectA, state, 4);
		manager.on("onChangeImm", state->objectA, state, 1);
		manager.on("onKeyframe", state->objectA, state, 1);
		manager.on("onExecute", state->objectA, state, 5);
		manager.on("once", state->objectA, state, 10);

		log::log(DBG << "##### SETUP DONE ");
		// Without anything happen and until time 0 nothing will happen
		{
			manager.execute_until(0, state); // Expected: onChangeImm(1) [ we changed data at t=1]
			TESTEQUALS(state->trace.empty(), true);
		}
		state->objectA->set_number(0, 1);
		{
			manager.execute_until(2, state); // Expected: nothing
			TESTEQUALS(state->trace.empty(), true);
		}
		{
			manager.execute_until(2, state); // Expected: nothing
			TESTEQUALS(state->trace.empty(), true);
		}
		log::log(DBG << "##### INIT DONE ");
		log::log(DBG << "Triggering Keyframe at 1");
		state->objectA->trigger_keyframe(1);
		{
			manager.execute_until(2, state); // Expected: onKeyframe(1)
			state->log_dbg();
			TESTEQUALS(state->trace.empty(), false);
			auto it = state->trace.begin();
			if (it->name != "onKeyframe")
				TESTFAILMSG("Unexpected Event: " << it->name << " expected onKeyframe");
			TESTEQUALS(it->time, 1);
			it ++;
			TESTEQUALS(it == state->trace.end(), true);
			state->trace.clear();
		}
		{
			manager.execute_until(5, state); // Expected: onChangeImm(2), onChange(2) and onExecute(5)
			TESTEQUALS(state->trace.empty(), false);
			auto it = state->trace.begin();
			if (it->name != "onChangeImm")
				TESTFAILMSG("Unexpected Event: " << it->name << " expected onChangeImm");
			TESTEQUALS(it->time, 2);
			it ++;
			if (it->name != "onChange")
				TESTFAILMSG("Unexpected Event: " << it->name << " expected onChange");
			TESTEQUALS(it->time, 3);
			it ++;
			TESTEQUALS(it == state->trace.end(), true);
			state->trace.clear();
		}
		{
			manager.execute_until(11, state); // Expected: onExecute(10), once(10)
			TESTEQUALS(state->trace.empty(), false);
			auto it = state->trace.begin();
			if (it->name != "onExecute")
				TESTFAILMSG("Unexpected Event: " << it->name << " expected onExecute");
			TESTEQUALS(it->time, 10);
			it ++;
			if (it->name != "once")
				TESTFAILMSG("Unexpected Event: " << it->name << " expected once");
			TESTEQUALS(it->time, 10);
			it ++;
			TESTEQUALS(it == state->trace.end(), true);
			state->trace.clear();
		}

		log::log(DBG << "changing the value at 12");
		state->objectA->set_number(1, 12);
		{
			manager.execute_until(15, state); // Expected: onChangeImm, onChange(12)
			TESTEQUALS(state->trace.empty(), false);
			auto it = state->trace.begin();
			if (it->name != "onChangeImm")
				TESTFAILMSG("Unexpected Event: " << it->name << " expected onChangeImm");
			TESTEQUALS(it->time, 13);
			it ++;
			if (it->name != "onChange")
				TESTFAILMSG("Unexpected Event: " << it->name << " expected onChange");
			TESTEQUALS(it->time, 14);
			it ++;
			TESTEQUALS(it == state->trace.end(), true);
			state->trace.clear();
		}
		{
			manager.execute_until(16, state); // Expected: onExecute(15)
			TESTEQUALS(state->trace.empty(), false);
			auto it = state->trace.begin();
			if (it->name != "onExecute")
				TESTFAILMSG("Unexpected Event: " << it->name << " expected onExecute");
			TESTEQUALS(it->time, 15);
			it ++;
			TESTEQUALS(it == state->trace.end(), true);
			state->trace.clear();
		}
	}
	log::log(DBG << "------------- [ Starting Test: Event parameter Mapping ] ------------");
    {
        class EventParameterMapTestClass : public curve::EventClass {
        public:
            EventParameterMapTestClass() : EventClass("EventParameterMap", EventClass::Type::ONCE) {}

            void setup(const std::shared_ptr<Event> &/*target*/,
                       const std::shared_ptr<State> &/*state*/) override {
            }

            void call(EventManager *,
                      const std::shared_ptr<EventTarget> &/*target*/,
                      const std::shared_ptr<State> &/*state*/,
                      const curve_time_t &/*time*/,
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
            curve_time_t recalculate_time(const std::shared_ptr<EventTarget> &/*target*/,
                                          const std::shared_ptr<State> &/*state*/,
                                          const curve::curve_time_t &at) override {
                return at;
            }
        };

        curve::EventManager manager;
        manager.add_class(std::make_shared<EventParameterMapTestClass>());
        auto state = std::make_shared<State>(&manager);
        manager.on("EventParameterMap", state->objectA, state, 1, {
                {"testInt", 1},
                {"testStdString", std::string("stdstring")},
                {"testString", "string"}});
        manager.execute_until(10, state);


    }
}

}}} // namespace openage::curve::tests
