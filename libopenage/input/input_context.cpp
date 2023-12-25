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

void InputContext::set_game_bindings(const std::shared_ptr<game::BindingContext> &bindings) {
	this->game_bindings = bindings;
}

void InputContext::set_camera_bindings(const std::shared_ptr<camera::BindingContext> &bindings) {
	this->camera_bindings = bindings;
}

void InputContext::set_hud_bindings(const std::shared_ptr<hud::BindingContext> &bindings) {
	this->hud_bindings = bindings;
}

const std::shared_ptr<game::BindingContext> &InputContext::get_game_bindings() {
	return this->game_bindings;
}

const std::shared_ptr<camera::BindingContext> &InputContext::get_camera_bindings() {
	return this->camera_bindings;
}

const std::shared_ptr<hud::BindingContext> &InputContext::get_hud_bindings() {
	return this->hud_bindings;
}

void InputContext::bind(const Event &ev, const input_action act) {
	std::vector<input_action> actions{act};
	this->by_event.emplace(std::make_pair(ev, actions));
}

void InputContext::bind(const event_class &cl, const input_action act) {
	std::vector<input_action> actions{act};
	this->by_class.emplace(std::make_pair(cl, actions));
}

void InputContext::bind(const Event &ev, const std::vector<input_action> &&acts) {
	this->by_event.emplace(std::make_pair(ev, std::move(acts)));
}

void InputContext::bind(const event_class &cl, const std::vector<input_action> &&acts) {
	this->by_class.emplace(std::make_pair(cl, std::move(acts)));
}

bool InputContext::is_bound(const Event &ev) const {
	return this->by_event.contains(ev) || this->by_class.contains(ev.cc.cl);
}

const std::vector<input_action> &InputContext::lookup(const Event &ev) const {
	auto event_lookup = this->by_event.find(ev);
	if (event_lookup != std::end(this->by_event)) {
		return (*event_lookup).second;
	}

	for (auto eclass : ev.cc.get_classes()) {
		auto class_lookup = this->by_class.find(eclass);
		if (class_lookup != std::end(this->by_class)) {
			return (*class_lookup).second;
		}
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
