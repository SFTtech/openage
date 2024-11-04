// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <concepts>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "gamestate/activity/node.h"
#include "gamestate/activity/types.h"
#include "time/time.h"


namespace openage::gamestate {
class GameEntity;

namespace activity {

/**
 * Chooses one of its output nodes based on enum values.
 *
 * In comparison to the XOR gate, this node type does not tie individual
 * conditions to each node. Instead, it operates on a single function that
 * returns a key for a lookup dict. The lookup dict maps these keys to output
 * node ID.
 *
 * This type of gate is easier to use for simpler branch switches based on
 * similar conditions, e.g. a branching based on the value of an enum.
 */
class XorSwitchGate : public Node {
public:
	/**
	 * Type used as lookup key for the lookup dict.
	 */
	using lookup_key_t = int;

	/**
	 * Function that retrieves a lookup key for the lookup dict from
	 * the current state of an entity.
	 *
	 * @param time Current simulation time.
	 * @param entity Entity that is executing the activity.
	 *
	 * @return Lookup key.
	 */
	using lookup_function_t = std::function<lookup_key_t(const time::time_t &,
	                                                     const std::shared_ptr<gamestate::GameEntity> &)>;

	/**
	 * Lookup dict that maps lookup keys to output node IDs.
	 */
	using lookup_dict_t = std::unordered_map<lookup_key_t, std::shared_ptr<Node>>;

	/**
	 * Creates a new XOR switch gate node.
	 *
	 * @param id Unique identifier of the node.
	 * @param label Human-readable label of the node (optional).
	 */
	XorSwitchGate(node_id_t id,
	              node_label_t label = "ExclusiveSwitchGateway");

	/**
	 * Creates a new XOR switch gate node.
	 *
	 * @param id Unique identifier of the node.
	 * @param label Human-readable label of the node.
	 * @param lookup_func Function that looks up the key to the lookup dict.
	 * @param lookup_dict Initial lookup dict that maps lookup keys to output node IDs.
	 * @param default_node Default output node. Chosen if no lookup entry is defined.
	 */
	XorSwitchGate(node_id_t id,
	              node_label_t label,
	              const lookup_function_t &lookup_func,
	              const lookup_dict_t &lookup_dict,
	              const std::shared_ptr<Node> &default_node);

	virtual ~XorSwitchGate() = default;

	inline node_t get_type() const override {
		return node_t::XOR_SWITCH_GATE;
	}

	/**
	 * Set the output node for a given lookup key.
	 *
	 * @param output Output node.
	 * @param key Enumeration value.
	 */
	void set_output(const std::shared_ptr<Node> &output,
	                const lookup_key_t &key);

	/**
	 * Get the lookup function for determining the output nodes.
	 *
	 * @return Lookup function.
	 */
	const lookup_function_t &get_lookup_func() const;

	/**
	 * Set the lookup function for determining the output nodes.
	 *
	 * @param lookup_func Lookup function.
	 */
	void set_lookup_func(const lookup_function_t &lookup_func);

	/**
	 * Get the lookup dict for the output nodes.
	 *
	 * @return Lookup dict.
	 */
	const lookup_dict_t &get_lookup_dict() const;

	/**
	 * Get the default output node.
	 *
	 * @return Default output node.
	 */
	const std::shared_ptr<Node> &get_default() const;

	/**
	 * Set the the default output node.
	 *
	 * This node is chosen if the lookup dict does not contain an entry for the
	 * lookup key returned by the lookup function.
	 *
	 * @param node Default output node.
	 */
	void set_default(const std::shared_ptr<Node> &node);

private:
	/**
	 * Determines the lookup key for the lookup dict from the current state.
	 */
	lookup_function_t lookup_func;

	/**
	 * Maps lookup keys to output node IDs.
	 */
	lookup_dict_t lookup_dict;

	/**
	 * Default output node. Chosen if no lookup entry is defined.
	 */
	std::shared_ptr<Node> default_node;
};

} // namespace activity
} // namespace openage::gamestate
