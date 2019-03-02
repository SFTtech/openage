// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#pragma once

namespace openage {

/**
 * Keeps track of the population size and capacity.
 */
class PopulationTracker {
public:

	PopulationTracker(int capacity_static, int capacity_max);

	/**
	 * Add to the population demand
	 */
	void demand_population(int i);

	/**
	 * Remove from the population demand
	 */
	void free_population(int i);

	/**
	 * Changes the capacity given by civ bonuses
	 */
	void add_capacity_static(int i);

	/**
	 * Add to the capacity given by units
	 */
	void add_capacity(int i);

	/**
	 * Remove from the capacity given by units
	 */
	void remove_capacity(int i);

	/**
	 * Changes the max capacity given by civ bonuses
	 */
	void add_capacity_max(int i);

	int get_demand() const;

	int get_capacity() const;

	/**
	 * Returns the available population capacity for new units.
	 */
	int get_space() const;

	/**
	 * Returns the population capacity over the max limit.
	 */
	int get_capacity_overflow() const;

	/**
	 * Check if the population capacity has reached the max limit.
	 */
	bool is_capacity_maxed() const;

private:

	/**
	 * Calculates the capacity values based on the limits.
	 * Must be called when a capacity variable changes.
	 */
	void update_capacity();

	/**
	 * The population demand
	 */
	int demand;

	/**
	 * The population capacity given by civ bonuses
	 */
	int capacity_static;

	/**
	 * The population capacity given by units
	 */
	int capacity_real;

	/**
	 * The max population capacity
	 */
	int capacity_max;

	// generated values

	/**
	 * All the population capacity without the limitation
	 */
	int capacity_total;

	/**
	 * All the population capacity with the limitation
	 */
	int capacity;

};

} // openage
