// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

namespace openage {

/**
 * Keeps track of the population size and capacity.
 */
class PopulationTracker {
public:

	PopulationTracker(int capacity_static, int capacity_max);

	void add_population(int i);

	/**
	 * Changes the capacity given by civ bonuses
	 */
	void add_capacity_static(int i);

	/**
	 * Changes the capacity given by units
	 */
	void add_capacity(int i);

	/**
	 * Changes the max capacity given by civ bonuses
	 */
	void add_capacity_max(int i);

	int get_population() const;

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

	int population;

	int capacity_static;
	int capacity_real;
	int capacity_max;

	// generated values

	int capacity_total;
	int capacity;

};

} // openage
