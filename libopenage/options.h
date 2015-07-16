// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_OPTIONS_H_
#define OPENAGE_OPTIONS_H_

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace openage {
namespace options {

class Variable {
public:
	Variable();

private:

};


using opt_func_t = std::function<void()>;

// TODO string description
// This is a base class for event_class, event, action_t mapped to a function
// This could be constructed with a context and bind itself using the required types
class OptionAction {
public:
	OptionAction(const std::string &name, const opt_func_t f);

	/**
	 * pass mouse position
	 */
	bool do_action();

	/**
	 * name of this action
	 */
	const std::string name;

private:
	const opt_func_t function;

};

/**
 * Allows objects to expose customisable features
 *
 * This is used to bind inner game classes
 * with console interaction or gui elements
 */
class OptionNode {
public:
	OptionNode(const std::string &panel_name);
	virtual ~OptionNode();

	/**
	 * lists all available options
	 */
	std::vector<std::string> list_options(bool recurse=false, std::string indent="");


	void do_action(const std::string &aname);

	/**
	 * return a child by name, or null if not available
	 */
	OptionNode *get_child(const std::string &name) const;

	/**
	 * read state of a bool
	 */
	bool get_bool(const std::string &vname, bool def=false) const;

	/**
	 * add types to the interface
	 */
	void add_bool(const std::string &vname, bool *value);
	void add_int(const std::string &vname, int *value);
	void add_action(const OptionAction &action);

	/**
	 * Set a parent panel which
	 * implies adding a child to the parent
	 */
	void set_parent(OptionNode *parent);


	const std::string name;

private:

	/**
	 * add child nodes
	 */
	void add_panel(OptionNode *child);

	/**
	 * remove child nodes
	 */
	void remove_panel(OptionNode *child);

	OptionNode *parent;

	// allow read only variables
	// variables which can be viewed
	std::unordered_map<std::string, std::shared_ptr<Variable>> vars;

	std::unordered_map<std::string, bool *> bools;
	std::unordered_map<std::string, int *> ints;


	// list available functions for interaction
	std::unordered_map<std::string, OptionAction> actions;


	std::unordered_map<std::string, OptionNode *> panels;
};

/**
 * A thing for drawing OptionPanels
 *
 * generates required gui elements for each
 * variable on a panel
 */
class OptionHud {
public:
	OptionHud();
};

} // namespace options
} // namespace openage

#endif
