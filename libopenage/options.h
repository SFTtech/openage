// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_OPTIONS_H_
#define OPENAGE_OPTIONS_H_

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "util/variable.h"

namespace openage {
namespace options {


/**
 * list of possible value types
 */
enum class option_type {
	bool_type,
	int_type,
	double_type,
	string_type,
	list_type
};

class OptionValue;

using option_list = std::vector<OptionValue>;

/**
 * stores a type and value
 */
class OptionValue {
public:
	OptionValue(bool b);
	OptionValue(int i);
	OptionValue(double d);
	OptionValue(const char *s);
	OptionValue(const std::string &s);
	OptionValue(const option_list &l);
	OptionValue(const OptionValue &v);

	/**
	 * check equality
	 */
	bool operator ==(const OptionValue &other) const;

	/**
	 * assignment
	 */
	const OptionValue &operator =(const OptionValue &other);

	/**
	 * type converted to a string
	 */
	std::string str_value() const;

	/**
	 * read inner type - the templated type must match
	 */
	template<class T>
	const T &value() const {
		return var->get<T>();
	}

	const option_type type;

private:

	/**
	 * set the value
	 */
	void set(const OptionValue &other);

	template<class T>
	void set_value(const OptionValue &other) {
		const T &other_value = other.value<T>();
		if (this->var) {
			this->var->set<T>(other_value);
		}
		else {
			this->var = std::make_unique<util::Variable<T>>(other_value);
		}
	}

	std::unique_ptr<util::VariableBase> var;

};

OptionValue parse(option_type t, std::string s);


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
	 * lists all available options in a readable format
	 */
	std::vector<std::string> list_options(bool recurse=false, std::string indent="");

	/**
	 * shows all available variable names
	 */
	std::vector<std::string> list_variables() const;

	std::vector<std::string> list_functions() const;

	OptionValue &get_variable_rw(const std::string &name);
	const OptionValue &get_variable(const std::string &name) const;

	/**
	 * shortcut for get_variable(name).value<T>()
	 */
	template<class T>
	const T &getv(const std::string &name) {
		return this->get_variable(name).value<T>();
	}

	/**
	 * return a child by name, or null if not available
	 */
	OptionNode *get_child(const std::string &name) const;

	/**
	 * Set a parent panel which
	 * implies adding a child to the parent
	 */
	void set_parent(OptionNode *parent);

	void do_action(const std::string &aname);

	/**
	 * read state of a bool
	 */
	bool get_bool(const std::string &vname, bool def=false) const;

	/**
	 * name of this node
	 */
	const std::string name;

protected:

	/**
	 * deprecated
	 */
	void add_bool(const std::string &vname, bool *value);
	void add_int(const std::string &vname, int *value);

	/**
	 * add types to the interface
	 */
	void add(const std::string &vname, const OptionValue &value);

	void add_action(const OptionAction &action);

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
	// variables which can be viewed:
	std::unordered_map<std::string, bool *> bools;
	std::unordered_map<std::string, int *> ints;

	std::unordered_map<std::string, OptionValue> varmap;


	// list available functions for interaction
	std::unordered_map<std::string, OptionAction> actions;


	std::unordered_map<std::string, OptionNode *> panels;
};

/**
 * A thing for drawing OptionNodes
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
