// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#include "options.h"
#include <utility>

namespace openage::options {


OptionValue::OptionValue(bool b)
	:
	type{option_type::bool_type},
	owner{true},
	var{new util::Variable<bool>(b)} {}


OptionValue::OptionValue(int i)
	:
	type{option_type::int_type},
	owner{true},
	var{new util::Variable<int>(i)} {}


OptionValue::OptionValue(double d)
	:
	type{option_type::double_type},
	owner{true},
	var{new util::Variable<double>(d)} {}


OptionValue::OptionValue(const char *s)
	:
	type{option_type::string_type},
	owner{true},
	var{new util::Variable<std::string>(std::string(s))} {}


OptionValue::OptionValue(const std::string &s)
	:
	type{option_type::string_type},
	owner{true},
	var{new util::Variable<std::string>(s)} {}


OptionValue::OptionValue(const option_list &l)
	:
	type{option_type::list_type},
	owner{true},
	var{new util::Variable<option_list>(l)} {}


OptionValue::OptionValue(const OptionValue &v)
	:
	type{v.type},
	owner{false},
	var{nullptr} {
	this->set(v);
}


OptionValue::OptionValue(util::Variable<bool> *b)
	:
	type{option_type::bool_type},
	owner{false},
	var{b} {}


OptionValue::OptionValue(util::Variable<int> *i)
	:
	type{option_type::int_type},
	owner{false},
	var{i} {}


OptionValue::OptionValue(util::Variable<double> *d)
	:
	type{option_type::double_type},
	owner{false},
	var{d} {}


OptionValue::OptionValue(util::Variable<std::string> *s)
	:
	type{option_type::string_type},
	owner{false},
	var{s} {}


OptionValue::OptionValue(util::Variable<option_list> *l)
	:
	type{option_type::list_type},
	owner{false},
	var{l} {}


OptionValue::~OptionValue() {
	if (this->owner) {
		delete this->var;
	}
}


bool OptionValue::is_reference() const {
	return !this->owner;
}


bool OptionValue::operator ==(const OptionValue &other) const {
	if (this->type != other.type) {
		return false;
	}

	// check each type
	switch (this->type) {

	case option_type::bool_type:
		return this->value<bool>() == other.value<bool>();

	case option_type::int_type:
		return this->value<int>() == other.value<int>();

	case option_type::double_type:
		return this->value<double>() == other.value<double>();

	case option_type::string_type:
		return this->value<std::string>() == other.value<std::string>();

	case option_type::list_type:
		return this->value<option_list>() == other.value<option_list>();
	default:
		return false;
	}
}


const OptionValue &OptionValue::operator =(const OptionValue &other) {
	if (this->type == other.type) {
		this->set(other);
	}
	return *this;
}


void OptionValue::set(const OptionValue &other) {
	switch (this->type) {

	case option_type::bool_type:
		this->set_value<bool>(other);
		break;

	case option_type::int_type:
		this->set_value<int>(other);
		break;

	case option_type::double_type:
		this->set_value<double>(other);
		break;

	case option_type::string_type:
		this->set_value<std::string>(other);
		break;

	case option_type::list_type:
		this->set_value<option_list>(other);
		break;

	}
}


std::string OptionValue::str_value() const {
	switch (this->type) {

	case option_type::bool_type:
		return this->value<bool>() ? "true" : "false";

	case option_type::int_type:
		return std::to_string(this->value<int>());

	case option_type::double_type:
		return std::to_string(this->value<double>());

	case option_type::string_type:
		return this->value<std::string>();

	case option_type::list_type: {
		std::string result = "[ ";
		for (auto &i : this->value<option_list>()) {
			result += i.str_value() + " ";
		}
		result += "]";
		return result;
	}
	default:
		return "";
	}
}


OptionValue parse(option_type t, const std::string &s) {
	switch(t) {
	case options::option_type::bool_type:
		return options::OptionValue(s == "true");

	case option_type::int_type:
		return options::OptionValue(stoi(s));

	case option_type::double_type:
		return options::OptionValue(stod(s));

	case option_type::string_type:
		return options::OptionValue(s);

	case option_type::list_type:
		// TODO: Missing case
		return options::OptionValue(false);

	default:
		return options::OptionValue(false);
	}
}


OptionAction::OptionAction(std::string name, opt_func_t f)
	:
	name{std::move(name)},
	function{std::move(f)} {
}


OptionValue OptionAction::do_action() {
	return this->function();
}


OptionNode::OptionNode(std::string panel_name)
	:
	name{std::move(panel_name)},
	parent{nullptr} {
}


OptionNode::~OptionNode() {
	if (this->parent) {
		this->parent->remove_panel(this);
	}
}


std::vector<std::string> OptionNode::list_options(bool recurse, const std::string &indent) {
	std::vector<std::string> result;
	result.push_back(indent + "node " + this->name + " {");
	std::string inner_indent = indent + "\t";
	for (auto &v : varmap) {
		result.push_back(inner_indent + "var " + v.first + " = " + v.second.str_value());
	}
	for (auto &f : actions) {
		result.push_back(inner_indent + "func " + f.first + "()");
	}
	for (auto &p : children) {
		for (auto &line : p.second->list_options(recurse, inner_indent)) {
			result.push_back(line);
		}
	}
	result.push_back(indent + "}");
	return result;
}


std::vector<std::string> OptionNode::list_variables() const {
	std::vector<std::string> result;
	for (auto &v : this->varmap) {
		result.push_back(v.first);
	}
	return result;
}


std::vector<std::string> OptionNode::list_functions() const {
	std::vector<std::string> result;
	for (auto &f : this->actions) {
		result.push_back(f.first);
	}
	return result;
}


OptionValue &OptionNode::get_variable_rw(const std::string &name) {
	return this->varmap.at(name);
}


const OptionValue &OptionNode::get_variable(const std::string &name) const {
	return this->varmap.at(name);
}


OptionNode *OptionNode::get_child(const std::string &name) const {
	auto child = this->children.find(name);
	if (child == this->children.end()) {
		return nullptr;
	}
	return child->second;
}


void OptionNode::set_parent(OptionNode *new_parent) {
	if (this->parent) {
		this->parent->remove_panel(this);
	}
	this->parent = new_parent;
	new_parent->add_panel(this);
}


OptionValue OptionNode::do_action(const std::string &aname) {
	auto a = this->actions.find(aname);
	if (a != this->actions.end()) {
		return a->second.do_action();
	}
	return OptionValue(false);
}


void OptionNode::add(const std::string &vname, const OptionValue &value) {
	this->varmap.emplace(std::make_pair(vname, value));
}


void OptionNode::add_action(const OptionAction &action) {
	this->actions.emplace(std::make_pair(action.name, action));
}


void OptionNode::add_panel(OptionNode *child) {
	this->children.emplace(std::make_pair(child->name, child));
}


void OptionNode::remove_panel(OptionNode *child) {
	this->children.erase(child->name);
}


OptionHud::OptionHud() = default;


} // namespace openage
