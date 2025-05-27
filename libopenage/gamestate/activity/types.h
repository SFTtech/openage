// Copyright 2023-2025 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <memory>

#include "time/time.h"


namespace nyan {
class Object;
} // namespace nyan

namespace openage {

namespace event {
class Event;
class EventLoop;
} // namespace event

namespace gamestate {
class GameEntity;
class GameState;

namespace activity {

/**
 * Node types in the flow graph.
 */
enum class node_t {
	START,
	END,
	XOR_EVENT_GATE,
	XOR_GATE,
	XOR_SWITCH_GATE,
	TASK_CUSTOM,
	TASK_SYSTEM,
};

/**
 * Create and register an event on the event loop.
 *
 * When the event is executed, the control flow continues on the branch
 * associated with the event.
 *
 * @param time Time at which the primer function is executed.
 * @param entity Game entity that the activity is assigned to.
 * @param loop Event loop that events are registered on.
 * @param state Game state.
 * @param next_id ID of the next node to visit. This is passed as an event parameter.
 *
 * @return Event registered on the event loop.
 */
using event_primer_t = std::function<std::shared_ptr<openage::event::Event>(const time::time_t &time,
                                                                            const std::shared_ptr<gamestate::GameEntity> &entity,
                                                                            const std::shared_ptr<event::EventLoop> &loop,
                                                                            const std::shared_ptr<gamestate::GameState> &state,
                                                                            size_t next_id)>;

/**
 * Function that determines if an output node is chosen.
 *
 * @param time Current simulation time.
 * @param entity Entity that is executing the activity.
 * @param api_object API object that is used to define the condition. May be used to
 *                   store additional data for evaluating the condition.
 *
 * @return true if the output node is chosen, false otherwise.
 */
using condition_t = std::function<bool(const time::time_t &time,
                                       const std::shared_ptr<gamestate::GameEntity> &entity,
                                       const std::shared_ptr<gamestate::GameState> &state,
                                       const std::shared_ptr<nyan::Object> &api_object)>;

/**
 * Condition used to determine if an output node is chosen.
 */
struct condition {
	/// API object for the condition definition.
	std::shared_ptr<nyan::Object> api_object;
	/// Checks whether the condition is true.
	/// TODO: We could look this function up at runtime.
	condition_t function;
};

/**
 * Type used as key for the node lookup dict of the XorSwitchGate.
 */
using switch_key_t = int;

/**
 * Function that retrieves a key for the node lookup dict of the
 * XorSwitchGate. The lookup key is calculated from the current state
 * of an entity.
 *
 * @param time Current simulation time.
 * @param entity Entity that is executing the activity.
 * @param api_object API object that is used to define the condition. May be used to
 *                   store additional data for evaluating the condition.
 *
 * @return Lookup key.
 */
using switch_function_t = std::function<switch_key_t(const time::time_t &time,
                                                     const std::shared_ptr<gamestate::GameEntity> &entity,
                                                     const std::shared_ptr<gamestate::GameState> &state,
                                                     const std::shared_ptr<nyan::Object> &api_object)>;

/**
 * Condition used to determine which output node of the
 * XorSwitchGate is chosen.
 */
struct switch_condition {
	/// API object for the condition definition.
	std::shared_ptr<nyan::Object> api_object;
	/// Returns the node lookup key for the output node that is chosen.
	/// TODO: We could look this function up at runtime.
	switch_function_t function;
};

} // namespace activity
} // namespace gamestate
} // namespace openage
