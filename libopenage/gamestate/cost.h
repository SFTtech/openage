// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#pragma once

#include "resource.h"


namespace openage {

class Player;

/**
 * Types of dynamic cost calculation (and one constant).
 *
 * Used in ResourceCost
 * \todo use in TimeCost
 */
enum class cost_type : int {
	/** Constant resources. */
	constant,
	/** Dynamic cost based on the workforce. */
	workforce
};

/**
 * A container for a constant or dynamic ResourceBundle representing the cost.
 */
class ResourceCost {
public:

	/**
	 * Constant zero cost
	 */
	ResourceCost();

	/**
	 * Constant cost
	 */
	ResourceCost(const ResourceBundle& resources);

	/**
	 * Dynamic cost
	 */
	ResourceCost(cost_type type, const ResourceBundle& multiplier);

	virtual ~ResourceCost();

	void set(cost_type type, const ResourceBundle& multiplier);

	/**
	 * Returns the cost.
	 */
	const ResourceBundle get(const Player& player) const;

private:

	cost_type type;

	ResourceBundle resources;

};

// \todo implement TimeCost

} // namespace openage
