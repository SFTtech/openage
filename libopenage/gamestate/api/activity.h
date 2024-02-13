// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include <nyan/nyan.h>

#include "gamestate/activity/types.h"
#include "gamestate/activity/xor_event_gate.h"
#include "gamestate/activity/xor_gate.h"
#include "gamestate/system/types.h"


namespace openage::gamestate {

namespace api {

/**
 * Helper class for creating Activity objects from the nyan API.
 */
class APIActivity {
public:
	/**
	 * Check if a nyan object is an Activity (type == \p engine.util.activity.Activity).
	 *
	 * @param obj nyan object.
	 *
	 * @return true if the object is an activity, else false.
	 */
	static bool is_activity(const nyan::Object &obj);

	/**
	 * Get the start node of an activity.
	 *
	 * @param activity nyan object.
	 *
	 * @return nyan object handle of the start node.
	 */
	static nyan::Object get_start(const nyan::Object &activity);
};

/**
 * Helper class for creating Activity node objects from the nyan API.
 */
class APIActivityNode {
public:
	/**
	 * Check if a nyan object is a node (type == \p engine.util.activity.node.Node).
	 *
	 * @param obj nyan object.
	 *
	 * @return true if the object is a node, else false.
	 */
	static bool is_node(const nyan::Object &obj);

	/**
	 * Get the type of a node.
	 *
	 * @param node nyan object.
	 *
	 * @return Type of the node.
	 */
	static activity::node_t get_type(const nyan::Object &node);

	/**
	 * Get the next nodes of a node.
	 *
	 * The number of next nodes depends on the type of the node and can range
	 * from 0 (end nodes) to n (gateways).
	 *
	 * @param node nyan object.
	 *
	 * @return nyan object handles of the next nodes.
	 */
	static std::vector<nyan::Object> get_next(const nyan::Object &node);

	/**
	 * Get the system id of an Ability node.
	 *
	 * @param node nyan object.
	 *
	 * @return System ID of the node.
	 */
	static system::system_id_t get_system_id(const nyan::Object &ability_node);
};

/**
 * Helper class for creating Activity condition objects from the nyan API.
 */
class APIActivityCondition {
public:
	/**
	 * Check if a nyan object is a condition (type == \p engine.util.activity.condition.Condition).
	 *
	 * @param obj nyan object.
	 *
	 * @return true if the object is a condition, else false.
	 */
	static bool is_condition(const nyan::Object &obj);

	/**
	 * Get the condition function for a condition.
	 *
	 * @param condition nyan object.
	 *
	 * @return Condition function.
	 */
	static activity::condition_t get_condition(const nyan::Object &condition);
};

/**
 * Helper class for creating Activity event objects from the nyan API.
 */
class APIActivityEvent {
public:
	/**
	 * Check if a nyan object is an event (type == \p engine.util.activity.event.Event).
	 *
	 * @param obj nyan object.
	 *
	 * @return true if the object is an event, else false.
	 */
	static bool is_event(const nyan::Object &obj);

	/**
	 * Get the primer function for an event type.
	 *
	 * @param event nyan object.
	 *
	 * @return Event primer function.
	 */
	static activity::event_primer_t get_primer(const nyan::Object &event);
};


} // namespace api
} // namespace openage::gamestate
