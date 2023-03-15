// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "input_context.h"


namespace openage::input {

InputContext::InputContext(const std::string id) :
	id{id},
	by_event{},
	by_class{} {}


const std::string &InputContext::get_id() {
	return this->id;
}

const std::shared_ptr<BindingContext> &InputContext::get_binding_context() {
	return this->binding_context;
}

void InputContext::bind(const Event &ev, const input_action act) {
	this->by_event.emplace(std::make_pair(ev, act));
}

void InputContext::bind(const event_class &cl, const input_action act) {
	this->by_class.emplace(std::make_pair(cl, act));
}

bool InputContext::is_bound(const Event &ev) const {
	return this->by_event.contains(ev) || this->by_class.contains(ev.cl);
}

const input_action &InputContext::lookup(const Event &ev) const {
	auto event_lookup = this->by_event.find(ev);
	if (event_lookup == std::end(this->by_event)) {
		return (*event_lookup).second;
	}

	auto class_lookup = this->by_class.find(ev.cl);
	if (class_lookup == std::end(this->by_class)) {
		return (*class_lookup).second;
	}

	throw Error{MSG(err) << "Event is not bound in context " << this->id};
}

std::vector<Event> InputContext::get_event_binds() const {
	std::vector<Event> result{};

	for (auto bind : this->by_event) {
		result.push_back(bind.first);
	}

	return result;
}

std::vector<event_class> InputContext::get_class_binds() const {
	std::vector<event_class> result{};

	for (auto bind : this->by_class) {
		result.push_back(bind.first);
	}

	return result;
}

} // namespace openage::input
