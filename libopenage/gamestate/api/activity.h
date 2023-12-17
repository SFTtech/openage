// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include <nyan/nyan.h>

#include "gamestate/activity/types.h"


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
};


} // namespace api
} // namespace openage::gamestate
