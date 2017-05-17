// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <string>
#include <memory>


namespace openage {

class Player;
class Research;
class ResourceBundle;

enum class research_category : int {
	/**
	 * Reseach which modify unit type data.
	 */
	unit_upgrade,
	/**
	 * Reseach which modify unit type data and also progresses the next age.
	 */
	age_advance,
	/**
	 * Reseach which modify unit type data and something else.
	 * (eg. see enemy line of sight)
	 */
	generic,
	RESEARCH_CATEGORY_COUNT
};

class ResearchType {
public:

	ResearchType(Player &owner);

	/**
	 * Gets the unique id of this research type.
	 */
	virtual int id() const = 0;

	/**
	 * Gets the name of the research.
	 */
	virtual std::string name() const = 0;

	/**
	 * Gets the research category of the research.
	 */
	virtual research_category category() const = 0;

	/**
	 * Creates a single Research object.
	 * Must be called only once.
	 */
	std::shared_ptr<Research> initialise() const;

	/**
	 * The player who owns this research type
	 */
	Player &owner;

	/**
	 * How many times it can be researched.
	 * All classic researches have a value of 1.
	 */
	int get_max_repeats() const { return 1; }

	virtual unsigned int get_research_time() const = 0;

	virtual ResourceBundle get_research_cost() const = 0;

	/**
	 * Performs the modifications (eg. apply patch to the unit types)
	 */
	virtual void apply() const = 0;

protected:

private:

};

class NyanResearchType : public ResearchType {
	// TODO POST-NYAN Implement
};

/**
 * At most one Research must exist for each ResearchType.
 */
class Research {
public:

	Research(const ResearchType *type);

	const ResearchType *type;

	/**
	 * Called when a unit started researching this research.
	 */
	void started();

	/**
	 * Called when a unit stopped researching this research before completing it.
	 */
	void stopped();

	/**
	 * Called when a unit completed researching this research.
	 */
	void completed();

	/**
	 * Returns true if a unit can start researching this research.
	 */
	bool can_start() const;

	/**
	 * Returns true if any unit is researching this research.
	 */
	bool is_active() const;

	/**
	 * Returns true if it has nothing more to offer (reached max repeats).
	 */
	bool is_researched() const;

	/**
	 * Apply the modifications to the owner player.
	 */
	void apply();

protected:

	/**
	 * The number of units that are researching this research
	 */
	int active_count;

	/**
	 * The number of times this research has been completed
	 */
	int completed_count;

private:

};

} // namespace openage
