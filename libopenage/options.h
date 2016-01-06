// Copyright 2015-2016 the openage authors. See copying.md for legal info.

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

/**
 * set the container used for lists
 */
using option_list = std::vector<OptionValue>;


/**
 * stores a type and value
 */
class OptionValue {
public:

	/**
	 * value ownership managed by this
	 */
	OptionValue(bool b);
	OptionValue(int i);
	OptionValue(double d);
	OptionValue(const char *s);
	OptionValue(const std::string &s);
	OptionValue(const option_list &l);
	OptionValue(const OptionValue &v);

	/**
	 * value owned by another object
	 */
	OptionValue(util::Variable<bool> *b);
	OptionValue(util::Variable<int> *i);
	OptionValue(util::Variable<double> *d);
	OptionValue(util::Variable<std::string> *s);
	OptionValue(util::Variable<option_list> *l);

	virtual ~OptionValue();

	/**
	 * True if this value references an object
	 * not owned by itself
	 */
	bool is_reference() const;

	/**
	 * Checks equality
	 */
	bool operator ==(const OptionValue &other) const;

	/**
	 * Assignment, reference values share their values
	 * non reference values are copied
	 */
	const OptionValue &operator =(const OptionValue &other);

	/**
	 * Value converted to a string
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
		else if (other.owner) {
			this->owner = true;
			this->var = new util::Variable<T>(other_value);
		}
		else {
			this->var = other.var;
		}
	}


	/**
	 * Use of shared_ptr and unique_ptr doesnt work here
	 * possibly solved by templating ownership type
	 *
	 * non-owned variables act as references
	 */
	bool owner;
	util::VariableBase *var;

};

OptionValue parse(option_type t, std::string s);


using opt_func_t = std::function<OptionValue()>;

// TODO string description
// This is a base class for event_class, event, action_t mapped to a function
// This could be constructed with a context and bind itself using the required types
class OptionAction {
public:
	OptionAction(const std::string &name, const opt_func_t f);

	/**
	 * pass mouse position
	 */
	OptionValue do_action();

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
	template<class T>
	friend class Var;
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

	/**
	 * shows all the available function names
	 */
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

	/**
	 * performs the named action
	 * returning the actions response
	 */
	OptionValue do_action(const std::string &aname);

	/**
	 * name of this node
	 */
	const std::string name;

protected:

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

	/**
	 * parent of this node
	 */
	OptionNode *parent;

	/**
	 * Variables which can be used
	 * TODO: read only variables
	 */
	std::unordered_map<std::string, OptionValue> varmap;


	// list available functions for interaction
	std::unordered_map<std::string, OptionAction> actions;

	/**
	 * children of this node
	 */
	std::unordered_map<std::string, OptionNode *> children;
};


/**
 * A interaface variable which gets monitored by an
 * option node allowing reflection, while also
 * being directly accessable as a typed member
 */
template<class T>
class Var : public util::Variable<T> {
public:
	Var(OptionNode *owner, const std::string &name, const T &init)
		:
		util::Variable<T>{init} {

		owner->add(name, this->value);
	}
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
