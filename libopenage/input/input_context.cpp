// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "input_context.h"


namespace openage::input {

InputContext::InputContext(const std::string id) :
	id{id},
	by_event{} {}


const std::string &InputContext::get_id() {
	return this->id;
}

const std::shared_ptr<BindingContext> &InputContext::get_binding_context() {
	return this->binding_context;
}

void InputContext::bind(const Event &ev, const InputAction act) {
	this->by_event.emplace(std::make_pair(ev, act));
}

bool InputContext::is_bound(const Event &ev) const {
	return this->by_event.contains(ev);
}

const InputAction &InputContext::lookup(const Event &ev) const {
	return this->by_event.at(ev);
}

} // namespace openage::input
