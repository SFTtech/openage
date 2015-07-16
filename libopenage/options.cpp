// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "options.h"

namespace openage {
namespace options {


Variable::Variable() {

}


OptionAction::OptionAction(const std::string &name, const opt_func_t f)
	:
	name{name},
	function{f} {
}


bool OptionAction::do_action() {
	this->function();
	return true;
}


OptionNode::OptionNode(const std::string &panel_name)
	:
	name{panel_name},
	parent{nullptr} {
}


OptionNode::~OptionNode() {
	if (this->parent) {
		this->parent->remove_panel(this);
	}
}


std::vector<std::string> OptionNode::list_options(bool recurse, std::string indent) {
	std::vector<std::string> result;
	result.push_back(indent + "OptionNode " + this->name + " {");
	std::string inner_indent = indent + "\t";
	for (auto &p : bools) {
		result.push_back(inner_indent + "bool " + p.first + " = " + (*p.second ? "true" : "false"));
	}
	for (auto &p : ints) {
		result.push_back(inner_indent + "int " + p.first + " = " + std::to_string(*p.second));
	}
	for (auto &p : panels) {
		for (auto &line : p.second->list_options(recurse, inner_indent)) {
			result.push_back(line);
		}
	}
	result.push_back(indent + "}");
	return result;
}


void OptionNode::do_action(const std::string &aname) {
	for (auto &a: this->actions) {
		a.second.do_action();
	}
}


OptionNode *OptionNode::get_child(const std::string &name) const {
	auto child = panels.find(name);
	if (child == panels.end()) {
		return nullptr;
	}
	return child->second;
}


bool OptionNode::get_bool(const std::string &vname, bool def) const {
	auto b = bools.find(vname);
	if (b != bools.end()) {
		return *b->second;
	}
	return def;
}


void OptionNode::add_bool(const std::string &vname, bool *value) {
	bools.emplace(std::make_pair(vname, value));
}


void OptionNode::add_int(const std::string &vname, int *value) {
	ints.emplace(std::make_pair(vname, value));
}


void OptionNode::add_action(const OptionAction &action) {
	actions.emplace(std::make_pair(action.name, action));
}


void OptionNode::set_parent(OptionNode *new_parent) {
	if (this->parent) {
		this->parent->remove_panel(this);
	}
	this->parent = new_parent;
	new_parent->add_panel(this);
}


void OptionNode::add_panel(OptionNode *child) {
	panels.emplace(std::make_pair(child->name, child));
}


void OptionNode::remove_panel(OptionNode *child) {
	panels.erase(child->name);
}


OptionHud::OptionHud() {

}


} // namespace options
} // namespace openage
