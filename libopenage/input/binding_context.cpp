// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "binding_context.h"


namespace openage::input {

BindingContext::BindingContext() :
	by_event{} {}

void BindingContext::bind(const Event &ev, const binding bind) {
	this->by_event.emplace(std::make_pair(ev, bind));
}

void BindingContext::bind(const event_class &cl, const binding bind) {
	this->by_class.emplace(std::make_pair(cl, bind));
}

bool BindingContext::is_bound(const Event &ev) const {
	return this->by_event.contains(ev) || this->by_class.contains(ev.cl);
}

const binding &BindingContext::lookup(const Event &ev) const {
	auto event_lookup = this->by_event.find(ev);
	if (event_lookup == std::end(this->by_event)) {
		return (*event_lookup).second;
	}

	auto class_lookup = this->by_class.find(ev.cl);
	if (class_lookup == std::end(this->by_class)) {
		return (*class_lookup).second;
	}

	throw Error{MSG(err) << "Event is not bound in binding context."};
}

} // namespace openage::input
