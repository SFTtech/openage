// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "binding_context.h"


namespace openage::input {

BindingContext::BindingContext() :
	by_event{} {}

void BindingContext::bind(const Event &ev, const Binding bind) {
	this->by_event.emplace(std::make_pair(ev, bind));
}

bool BindingContext::is_bound(const Event &ev) const {
	return this->by_event.contains(ev);
}

const Binding &BindingContext::lookup(const Event &ev) const {
	return this->by_event.at(ev);
}

} // namespace openage::input
